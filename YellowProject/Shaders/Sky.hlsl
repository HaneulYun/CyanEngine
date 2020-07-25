#include "cbPass.hlsl"
#include "common.hlsl"

struct VertexIn
{
	float3 PosL		: POSITION;
	float3 NormalL	: NORMAL;
	float2 TexC		: TEXCOORD;
	float3 TangentL : TANGENT;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

struct MRT_VSOutput
{
	float4 Diffuse : SV_TARGET0;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
	VertexOut vout;

	vout.PosL = vin.PosL;
	float4 posW = mul(float4(vin.PosL, 1.0f), gInstanceData[instanceID].World);
	posW.xyz += gEyePosW;

	vout.PosH = mul(posW, gViewProj).xyww;

	return vout;
}

MRT_VSOutput PS(VertexOut pin)
{
	MRT_VSOutput result;
	result.Diffuse = gCubeMap.Sample(gsamLinearWrap, pin.PosL);
	result.Diffuse.w = 1000;
	
	return result;;
}