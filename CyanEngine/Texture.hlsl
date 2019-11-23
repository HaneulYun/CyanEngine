struct MEMORY
{
	matrix transform;
	float4 color;
};

StructuredBuffer<MEMORY> instance : register(t0);

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

#define _WITH_VERTEX_LIGHTING

struct VS_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct VS_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#else
	float3 normalW : NORMAL;
#endif
};

#include "Light.hlsl"


VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_LIGHTING_OUTPUT output;

	output.positionW = (float3)mul(float4(input.position, 1.0f), instance[nInstanceID].transform);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	float3 normalW = mul(input.normal, (float3x3)instance[nInstanceID].transform);
#ifdef _WITH_VERTEX_LIGHTING
	output.color = instance[nInstanceID].color * Lighting(output.positionW, normalize(normalW));
#else
	output.normalW = normalW;
#endif
	return(output);
}

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
	return(input.color);
#else
	float3 normalW = normalize(input.normalW);
	float4 color = Lighting(input.positionW, normalW);
	return(color);
#endif
}