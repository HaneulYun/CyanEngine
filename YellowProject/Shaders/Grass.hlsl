#include "shaders\\Common.hlsl"

struct VertexIn
{
	float3 PosL : POSITION;
	float2 SizeW : SIZE;
	float3 Look : LOOK;
};

struct VertexOut
{
	float3 CenterW : POSITION;
	float2 SizeW : SIZE;
	float3 Look : LOOK;

	nointerpolation uint MatIndex : MATINDEX;
};

struct GeoOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC : TEXCOORD;
	uint   PrimID : SV_PrimitiveID;
	float4 ShadowPosH : DD;

	nointerpolation uint MatIndex : MATINDEX;
};

float CalcShadowFactor(float4 shadowPosH)
{
	// Complete projection by doing division by w.
	shadowPosH.xyz /= shadowPosH.w;

	// Depth in NDC space.
	float depth = shadowPosH.z;

	uint width, height, numMips;
	gShadowMap.GetDimensions(0, width, height, numMips);

	// Texel size.
	float dx = 1.0f / (float)width;

	float percentLit = 0.0f;
	const float2 offsets[9] =
	{
		float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
	};

	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		percentLit += gShadowMap.SampleCmpLevelZero(gsamShadow,
			shadowPosH.xy + offsets[i], depth).r;
	}

	return percentLit / 9.0f;
}

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
	InstanceData instData = gInstanceData[instanceID];

	VertexOut vout;
	vout.MatIndex = gMaterialIndexData[instanceID * instData.MaterialIndexStride].MaterialIndex;
	vout.CenterW = mul(float4(vin.PosL, 1.0f), gInstanceData[instanceID].World).xyz;
	vout.SizeW = vin.SizeW;
	vout.Look = vin.Look;
	return vout;
}

[maxvertexcount(8)]
void GS(point VertexOut gin[1],
	uint primID : SV_PrimitiveID,
	inout TriangleStream<GeoOut> triStream)
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

	GeoOut gout;
	[unroll]
	for (int i = 0; i < 8; ++i)
	{
		if (i % 2 == 1)
			v[i].x += sin(gTotalTime + v[i].x / 10);
		gout.PosH = mul(v[i], gViewProj);
		gout.PosW = v[i].xyz;
		gout.NormalW = gin[0].Look;
		gout.TexC = texC[i % 4];
		gout.PrimID = primID;
		gout.MatIndex = gin[0].MatIndex;
		gout.ShadowPosH = mul(gout.PosW, gShadowTransform);
		triStream.Append(gout);
	}
}

float4 PS(GeoOut pin) : SV_Target
{
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

	float3 toEyeW = gEyePosW - pin.PosW;
	float distToEye = length(toEyeW);
	toEyeW /= distToEye;

	float4 ambient = gAmbientLight * diffuseAlbedo;

	const float shininess = 1.0f - roughness;
	Material mat = { diffuseAlbedo, fresnelR0, shininess };

	//float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
	//shadowFactor[0] = CalcShadowFactor(pin.ShadowPosH);
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
		pin.NormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

#ifdef FOG
	float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
	litColor = lerp(litColor, gFogColor, fogAmount);
#endif

	litColor.a = diffuseAlbedo.a;

	return litColor;
}