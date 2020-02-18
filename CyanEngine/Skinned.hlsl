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

cbuffer cbSkinned : register(b0)
{
	float4x4 gBoneTransforms[96];
};

#define _WITH_VERTEX_SKINNED

struct VS_SKINNED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 BoneWeights : WEIGHTS;
	uint4 BoneIndices  : BONEINDICES;
};

struct VS_SKINNED_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
#ifdef _WITH_VERTEX_SKINNED
	float4 color : COLOR;
#else
	float3 normalW : NORMAL;
#endif
	float2 uv : TEXCOORD;
};


#include "Light.hlsl"


VS_SKINNED_OUTPUT VSTextured(VS_SKINNED_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_SKINNED_OUTPUT output;
	
	// skinned
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = input.BoneWeights.x;
	weights[1] = input.BoneWeights.y;
	weights[2] = input.BoneWeights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];
	if (input.BoneIndices[3] == 0)
		weights[3] = 0;
	float3 posL = float3(0.0f, 0.0f, 0.0f);
	float3 normalL = float3(0.0f, 0.0f, 0.0f);
	float3 tangentL = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; ++i)
	{
		// Assume no nonuniform scaling when transforming normals, so 
		// that we do not have to use the inverse-transpose.

		posL += weights[i] * mul(float4(input.position, 1.0f), gBoneTransforms[input.BoneIndices[i]]).xyz;
		normalL += weights[i] * mul(input.normal, (float3x3)gBoneTransforms[input.BoneIndices[i]]);
		//tangentL += weights[i] * mul(input.TangentL.xyz, (float3x3)gBoneTransforms[input.BoneIndices[i]]);
	}

	input.position = posL;
	input.normal = normalL;
	//input.TangentL.xyz = tangentL;
	// skinned
	
	output.positionW = (float3)mul(float4(input.position, 1.0f), instance[nInstanceID].transform);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	float3 normalW = mul(input.normal, (float3x3)instance[nInstanceID].transform);
#ifdef _WITH_VERTEX_SKINNED
	output.color = instance[nInstanceID].color * Lighting(output.positionW, normalize(normalW));
#else
	output.normalW = normalW;
#endif
	output.uv = input.uv;
	return(output);
}

float4 PSTextured(VS_SKINNED_OUTPUT input) : SV_TARGET
{
	//return float4(input.uv.x, input.uv.y, 0, 1);
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
	return(cColor);
#ifdef _WITH_VERTEX_SKINNED
	return(input.color);
#else
	float3 normalW = normalize(input.normalW);
	float4 color = Lighting(input.positionW, normalW);

	return(color);
#endif
}