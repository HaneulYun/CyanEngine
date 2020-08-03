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
Texture2D gShadowMap[4] : register(t0, space3);

Texture2D gDiffuseMap[256-20] : register(t1, space2);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);

float CalcShadowFactor(float4 shadowPosH, int index)
{
	// Complete projection by doing division by w.
	shadowPosH.xyz /= shadowPosH.w;

	// Depth in NDC space.
	float depth = shadowPosH.z;

	uint width, height, numMips;
	gShadowMap[index].GetDimensions(0, width, height, numMips);

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
		percentLit += gShadowMap[index].SampleCmpLevelZero(gsamShadow,
			shadowPosH.xy + offsets[i], depth).r;
	}

	return percentLit / 9.0f;
}