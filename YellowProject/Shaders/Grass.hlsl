#include "common.hlsl"

struct VSInput
{
	float3 PosL : POSITION;
	float2 SizeW : SIZE;
	float3 Look : LOOK;
};

struct GSInput
{
	float3 CenterW : POSITION;
	float2 SizeW : SIZE;
	float3 Look : LOOK;

	nointerpolation uint MatIndex : MATINDEX;
};

struct PSInput
{
	float4 PosH : SV_POSITION;
	float4 ShadowPosH : POSITION0;
	float3 PosW : POSITION1;
	float3 NormalW : NORMAL;
	float2 TexC : TEXCOORD;
	uint   PrimID : SV_PrimitiveID;

	nointerpolation uint MatIndex : MATINDEX;
};

GSInput VS(VSInput vin, uint instanceID : SV_InstanceID)
{
	InstanceData instData = gInstanceData[instanceID];

	GSInput vout;
	vout.MatIndex = gMaterialIndexData[instanceID * instData.MaterialIndexStride].MaterialIndex;
	vout.CenterW = mul(float4(vin.PosL, 1.0f), gInstanceData[instanceID].World).xyz;
	vout.SizeW = vin.SizeW;
	vout.Look = normalize(vin.Look);
	return vout;
}

[maxvertexcount(8)]
void GS(point GSInput gin[1],
	uint primID : SV_PrimitiveID,
	inout TriangleStream<PSInput> triStream)
{
	float4x4 r = {
		0,0,-1,0,
		0,1,0,0,
		1,0,0,0,
		0,0,0,1
	};

	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 right = cross(up, gin[0].Look);
	float halfWidth = 0.5f * gin[0].SizeW.x;
	float halfHeight = 0.5f * gin[0].SizeW.y;

	float4 v[8];
	v[0] = float4(gin[0].CenterW + halfWidth * right - halfHeight * up, 1.0f);
	v[1] = float4(gin[0].CenterW + halfWidth * right + halfHeight * up, 1.0f);
	v[2] = float4(gin[0].CenterW - halfWidth * right - halfHeight * up, 1.0f);
	v[3] = float4(gin[0].CenterW - halfWidth * right + halfHeight * up, 1.0f);
	right = mul(float4(right, 1.0f), r);
	v[4] = float4(gin[0].CenterW + halfWidth * right - halfHeight * up, 1.0f);
	v[5] = float4(gin[0].CenterW + halfWidth * right + halfHeight * up, 1.0f);
	v[6] = float4(gin[0].CenterW - halfWidth * right - halfHeight * up, 1.0f);
	v[7] = float4(gin[0].CenterW - halfWidth * right + halfHeight * up, 1.0f);

	float2 texC[4] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
	};

	PSInput gout;
	[unroll]
	for (int i = 0; i < 8; ++i)
	{
		if (i % 2 == 1)
			v[i].x += sin(gTotalTime * 0.5f + v[i].x / 20) * 0.5f;
		gout.PosH = mul(v[i], gViewProj);
		gout.PosW = v[i].xyz;
		if (i % 4 == 0)
		{
			int n = (i / 4) * 4;
			float3 look = cross(v[n + 1] - v[n + 0], v[n + 2] - v[n + 1]);

			float3 toEye = normalize(gEyePosW - gout.PosW);
			if (dot(look, toEye) < 0)
				look = -look;

			gin[0].Look = look;
		}
		gout.NormalW = gin[0].Look;
		gout.TexC = texC[i % 4];
		gout.PrimID = primID;
		gout.MatIndex = gin[0].MatIndex;
		gout.ShadowPosH = mul(v[i], gShadowTransform);
		if (i % 4 == 0)
			triStream.RestartStrip();
		triStream.Append(gout);
	}
}

struct MRT_VSOutput
{
	float4 Color : SV_TARGET0;
	float4 Diffuse : SV_TARGET1;
	float4 Normal : SV_TARGET2;
	float4 SpecPow : SV_TARGET3;
};

MRT_VSOutput PS(PSInput pin)
{
	clip(pin.TexC.y - 0.1);

	MaterialData matData = gMaterialData[pin.MatIndex];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;
	float3 fresnelR0 = matData.FresnelR0;
	float roughness = matData.Roughness;
	uint diffuseTexIndex = matData.DiffuseMapIndex;
	float3 uvw = float3(pin.TexC, pin.PrimID % 3);

	diffuseAlbedo *= gDiffuseMap[diffuseTexIndex].Sample(gsamAnisotropicWrap, uvw);

#ifdef ALPHA_TEST
	clip(diffuseAlbedo.a - 0.1f);
#endif

	pin.NormalW = normalize(pin.NormalW);

	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	float4 ambient = gAmbientLight * diffuseAlbedo;


	float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
	shadowFactor[0] = CalcShadowFactor(pin.ShadowPosH);
	const float shininess = 1.0f - roughness;
	Material mat = { diffuseAlbedo, fresnelR0, shininess };

	float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
		pin.NormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

#ifdef FOG
	float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
	litColor = lerp(litColor, gFogColor, fogAmount);
#endif

	litColor.a = diffuseAlbedo.a;

	//return litColor;

	MRT_VSOutput result;
	result.Color = litColor;
	result.Diffuse = diffuseAlbedo;
	result.Normal = float4(pin.NormalW, 1);
	result.SpecPow = float4(pin.NormalW, 1);

	return result;
}