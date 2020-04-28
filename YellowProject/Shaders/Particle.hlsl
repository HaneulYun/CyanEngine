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
};

struct PIn
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
	float3 NormalW	: NORMAL;
	float2 TexC		: TEXCOORD;
	uint PrimID		: SV_PrimitiveID;
};

GIn VSMain(VIn vin)
{
	GIn gin;

	gin.CenterW = vin.PosW;
	gin.SizeW = vin.SizeW;
	gin.LifeTime = vin.LifeTime;
	gin.Dir = vin.Dir;
	gin.Speed = vin.Speed;
	gin.Type = vin.Type;
;
	return gin;
}

[maxvertexcount(4)]
void GSMain(point GIn gin[1], uint primID : SV_PrimitiveID, inout TriangleStream<PIn> triStream)
{
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 look = gEyePosW - gin[0].CenterW;
	look.y = 0.0f;
	look = normalize(look);
	float3 right = cross(up, look);
	
	float halfWidth = 1;
	float halfHeight = 1;
	
	float4 v[4];
	v[0] = float4(gin[0].CenterW + halfWidth * right - halfHeight * up, 1.0f);
	v[1] = float4(gin[0].CenterW + halfWidth * right + halfHeight * up, 1.0f);
	v[2] = float4(gin[0].CenterW - halfWidth * right - halfHeight * up, 1.0f);
	v[3] = float4(gin[0].CenterW - halfWidth * right + halfHeight * up, 1.0f);
	
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
		pin.PosH = mul(v[i], gViewProj);
		pin.PosW = v[i].xyz;
		pin.NormalW = look;
		pin.TexC = texC[i];
		pin.PrimID = primID;
	
		triStream.Append(pin);
	}
}

[maxvertexcount(4)]
void GSParticleMaker(point VIn vin[1], inout PointStream<VIn> pointStream)
{
	vin[0].LifeTime -= gDeltaTime;

	if (vin[0].Type == 0)
	{
		if (vin[0].LifeTime < 0)
		{
			vin[0].LifeTime = 0.2;

			VIn vout;
			vout = vin[0];

			vout.Type = 1;
			vout.LifeTime = 2;

			pointStream.Append(vout);
		}
		pointStream.Append(vin[0]);
	}
	if (vin[0].Type == 1)
	{
		if (vin[0].LifeTime > 0)
		{
			VIn vout;
			vout = vin[0];
			vout.PosW.x += 0.01;
			pointStream.Append(vout);
		}
	}
}

float4 PSMain(PIn input) : SV_TARGET
{
	return float4(1, 0, 0, 1);
}