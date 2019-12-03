struct MEMORY
{
	matrix transform;
	float4 color;
};

StructuredBuffer<MEMORY> instance : register(t0);
Texture2D gtxtTexture : register(t1);
SamplerState gSamplerState : register(s0);

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	float3 gvCameraPosition : packoffset(c8);
};

#define _WITH_VERTEX_LIGHTING

struct VS_LIGHTING_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
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
	float2 uv : TEXCOORD;
};

#include "Light.hlsl"


VS_LIGHTING_OUTPUT VSTextured(VS_LIGHTING_INPUT input, uint nInstanceID : SV_InstanceID)
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
	output.uv = input.uv;
	return(output);
}

float4 PSTextured(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
	//return float4(input.uv.x, input.uv.y, 0, 1);
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
	return(cColor);
#ifdef _WITH_VERTEX_LIGHTING
	return(input.color);
#else
	float3 normalW = normalize(input.normalW);
	float4 color = Lighting(input.positionW, normalW);

	return(color);
#endif
}

float4 PSSkyBox(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);

	return(cColor);
}