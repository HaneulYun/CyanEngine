#include "shaders\\Common.hlsl"

struct VIn
{
	float3 PosW		: POSITION;
	float2 SizeW	: SIZE;
	float LifeTime  : TIME;
	float3 Dir		: DIRECTION;
	float Speed		: SPEED;
	uint Type		: TYPE;
};

struct GIn
{
	float3 CenterW	: POSITION;
	float2 SizeW	: SIZE;
	float LifeTime  : TIME;
	float3 Dir		: DIRECTION;
	float Speed		: SPEED;
	uint Type		: TYPE;
	uint ObjIndex	: ID;
	nointerpolation uint MatIndex : MATINDEX;
};

struct PIn
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
	float3 NormalW	: NORMAL;
	float2 TexC		: TEXCOORD;
	float alpha		: ALPHA;
	uint PrimID		: SV_PrimitiveID;
	nointerpolation uint MatIndex : MATINDEX;
};

GIn VSMain(VIn vin, uint instanceID : SV_InstanceID)
{
	GIn gin;

	InstanceData instData = gInstanceData[instanceID];

	gin.CenterW = vin.PosW;
	gin.SizeW = vin.SizeW;
	gin.LifeTime = vin.LifeTime;
	gin.Dir = vin.Dir;
	gin.Speed = vin.Speed;
	gin.Type = vin.Type;
	gin.MatIndex = gMaterialIndexData[instanceID * instData.MaterialIndexStride].MaterialIndex;
	gin.ObjIndex = instanceID;
;
	return gin;
}

[maxvertexcount(4)]
void GSMain(point GIn gin[1], uint primID : SV_PrimitiveID, inout TriangleStream<PIn> triStream)
{
	if (gin[0].Type == 0)
		return;
	float3 look = gEyePosW - gin[0].CenterW;
	
	float4 v[4];

	InstanceData instData = gInstanceData[gin[0].ObjIndex];
	float4x4 world = instData.World;
	float4 posW = mul(float4(gin[0].CenterW, 1.0f), world);
	float4 pos = mul(posW, gViewProj);
	v[0] = pos + float4(-2, -2, 0, 1);
	v[1] = pos + float4(-2,  2, 0, 1);
	v[2] = pos + float4( 2, -2, 0, 1);
	v[3] = pos + float4( 2,  2, 0, 1);
	
	float2 texC[4] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
	};
	
	PIn pin;
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		pin.PosH = v[i];
		pin.PosW = v[i].xyz;
		pin.NormalW = look;
		pin.TexC = texC[i];
		pin.PrimID = primID;
		pin.alpha = gin[0].LifeTime / 2;
		pin.MatIndex = gin[0].MatIndex;
	
		triStream.Append(pin);
	}
}

[maxvertexcount(4)]
void GSParticleMaker(point VIn vin[1], inout PointStream<VIn> pointStream)
{
	vin[0].LifeTime -= gDeltaTime;

	if (vin[0].Type == 0)
	{
		VIn vout;
		bool b = false;
		if (gInstanceData[0].ObjPad0 && vin[0].LifeTime < 0)
		{
			vin[0].LifeTime = 0.2;

			vout = vin[0];

			vout.Type = 1;
			vout.LifeTime = 2;

			float x = ((gDeltaTime * 10000000) % 100 - 50) / 50;
			float y = ((gDeltaTime * 100000000) % 100) / 100;
			float z = ((gDeltaTime * 1000000) % 100 - 50) / 50;
			vout.Dir = normalize(float3(x, y, z));
			vout.Speed = 2;

			b = true;
		}
		pointStream.Append(vin[0]);
		if(b)
			pointStream.Append(vout);
	}
	if (vin[0].Type == 1)
	{
		if (vin[0].LifeTime > 0)
		{
			VIn vout;
			vout = vin[0];
			vout.Speed = vout.LifeTime * vout.LifeTime;
			vout.PosW += vout.Dir * vout.Speed * gDeltaTime;;
			pointStream.Append(vout);
		}
	}
}

float4 PSMain(PIn input) : SV_TARGET
{
	//float x = ((gDeltaTime * 10000000) % 100) / 100;
	//return float4(x, x, x, 1);

	MaterialData matData = gMaterialData[input.MatIndex];
	//float4 diffuseAlbedo = matData.DiffuseAlbedo;
	//float3 fresnelR0 = matData.FresnelR0;
	//float roughness = matData.Roughness;
	uint diffuseTexIndex = matData.DiffuseMapIndex;

	//diffuseAlbedo *= 
	float4 result = gDiffuseMap[diffuseTexIndex].Sample(gsamAnisotropicWrap, input.TexC);
	result.a *= input.alpha;

	clip(result.a - 0.01f);

	return result;
	//return diffuseAlbedo;
}