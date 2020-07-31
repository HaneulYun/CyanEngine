#include "cbPass.hlsl"
#include "common.hlsl"

cbuffer LightDirection : register(b1)
{
	float3	Strength;
	float	FalloffStart;
	float3	Direction;
	float	FalloffEnd;
	float3	Position;
	float	SpotPower;
	uint	Type;
	uint3	Pad;
};

struct VertexIn
{
	float3 PosL		: POSITION;
	float3 NormalL	: NORMAL;
	float2 TexC		: TEXCOORD;
	float3 TangentL : TANGENT;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 NormalL	: NORMAL;
	float3 PosL		: POSITION;
};

struct MRT_VSOutput
{
	float4 Diffuse : SV_TARGET0;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
	VertexOut vout;

	vout.PosL = vin.PosL;
	vout.NormalL = vin.NormalL;
	float4 posW = mul(float4(vin.PosL, 1.0f), gInstanceData[instanceID].World);
	posW.xyz += gEyePosW;

	vout.PosH = mul(posW, gViewProj).xyww;

	return vout;
}

MRT_VSOutput PS(VertexOut pin)
{
	MRT_VSOutput result;
	result.Diffuse.xyz = gCubeMap.Sample(gsamLinearWrap, pin.PosL) * Strength;
	result.Diffuse.w = 1000;

	float3 toLight = normalize(-Direction);
	pin.NormalL = normalize(pin.NormalL);

	float d = dot(toLight, pin.NormalL);
	if (d > 0.994)
		result.Diffuse.xyz += 1;
	else if (d > 0.990)
		result.Diffuse.xyz += 1 - ((0.994 - d) / 0.004);
	
	return result;;
}