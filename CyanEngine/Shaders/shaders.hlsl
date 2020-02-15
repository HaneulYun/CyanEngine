#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 1
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

#include "..\\CyanEngine\\shaders\\LightingUtil.hlsl"

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
}

cbuffer cbMaterial : register(b1)
{
	float4 gDiffuseAlbedo;
	float3 gFresnelR0;
	float  gRoughness;
	float4x4 gMatTransform;
}

cbuffer cbPass : register(b2)
{
	float4x4 gView;
	float4x4 gInvView;
	float4x4 gProj;
	float4x4 gInvProj;
	float4x4 gViewProj;
	float4x4 gInvViewProj;

	float3 gEyePosW;
	float  cbPerObjectPad1;
	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;

	float gNearZ;
	float gFarZ;
	float gTotalTime;
	float gDeltaTime;

	float4 gAmbientLight;
	Light gLights[MaxLights];
}

struct PSInput
{
	float4 position : SV_POSITION;
	float4 posW : POSITION;
	float3 normal : NORMAL;
};

PSInput VSMain(float3 position : POSITION, float3 normal : NORMAL)
{
	PSInput result;

	result.posW = mul(float4(position, 1.0f), gWorld);
	result.position = mul(result.posW, gViewProj);
	result.normal = normal;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float3 toEyeW = normalize(gEyePosW - input.posW.xyz);
	float4 ambient = gAmbientLight * gDiffuseAlbedo;

	const float shininess = 1.0f - gRoughness;
	Material mat = { gDiffuseAlbedo, gFresnelR0, shininess };
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLights, mat, input.position.xyz, input.normal, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	litColor.a = gDiffuseAlbedo.a;

	return litColor;
}