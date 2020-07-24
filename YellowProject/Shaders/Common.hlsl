#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

#include "lightingUtil.hlsl"

struct InstanceData
{
	float4x4 World;
	float4x4 TexTransform;
	uint MaterialIndexStride;
	uint BoneTransformStride;
	uint ObjPad0;
	uint ObjPad1;
};

struct MaterialData
{
	float4	 DiffuseAlbedo;
	float3	 FresnelR0;
	float	 Roughness;
	float4x4 MatTransform;
	uint	 DiffuseMapIndex;
	uint	 NormalMapIndex;
	uint	 MatPad1;
	uint	 MatPad2;
};

struct MatIndexData
{
	uint	MaterialIndex;
};

struct BoneTransform
{
	float4x4 BoneTransforms;
};

StructuredBuffer<InstanceData> gInstanceData : register(t0);
StructuredBuffer<MatIndexData> gMaterialIndexData : register(t1);
StructuredBuffer<BoneTransform> gSkinnedData : register(t2);

StructuredBuffer<MaterialData> gMaterialData : register(t0, space2);

TextureCube gCubeMap : register(t0, space1);
Texture2D gBufferMap[7] : register(t1, space1);
Texture2D gShadowMap : register(t0, space3);

Texture2D gDiffuseMap[16] : register(t1, space2);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);

cbuffer cbPass : register(b0)
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