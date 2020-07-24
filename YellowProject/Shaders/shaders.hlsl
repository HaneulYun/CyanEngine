#include "common.hlsl"

struct VSInput
{
	float3 PosL		: POSITION;
	float3 NormalL	: NORMAL;
	float2 TexC		: TEXCOORD;
	float3 TangentL : TANGENT;
#ifdef SKINNED
	float3 BoneWeights : WEIGHTS;
	uint4 BoneIndices  : BONEINDICES;
#endif
};

struct PSInput
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION1;
	float3 NormalW	: NORMAL;
	float3 TangentW : TANGENT;
	float2 TexC		: TEXCOORD;

	nointerpolation uint MatIndex : MATINDEX;
};

struct MRT_VSOutput
{
	float4 Diffuse : SV_TARGET0;
	float4 Normal : SV_TARGET1;
};

PSInput VS(VSInput vin, uint instanceID : SV_InstanceID)
{
	PSInput vout;

	InstanceData instData = gInstanceData[instanceID];
	float4x4 world = instData.World;
	float4x4 texTransform = instData.TexTransform;
	uint matIndex = gMaterialIndexData[instanceID * instData.MaterialIndexStride].MaterialIndex;

	vout.MatIndex = matIndex;

	MaterialData matData = gMaterialData[matIndex];

#ifdef SKINNED
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = vin.BoneWeights.x;
	weights[1] = vin.BoneWeights.y;
	weights[2] = vin.BoneWeights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	float3 posL = float3(0.0f, 0.0f, 0.0f);
	float3 normalL = float3(0.0f, 0.0f, 0.0f);
	float3 tangentL = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; ++i)
	{
		int boneIndex = vin.BoneIndices[i] + instanceID * instData.BoneTransformStride;
		float4x4 boneTransform = gSkinnedData[boneIndex].BoneTransforms;
		posL += weights[i] * mul(float4(vin.PosL, 1.0f), boneTransform).xyz;
		normalL += weights[i] * mul(vin.NormalL, (float3x3)boneTransform);
		tangentL += weights[i] * mul(vin.TangentL.xyz, (float3x3)boneTransform);
	}

	vin.PosL = posL;
	vin.NormalL = normalL;
	vin.TangentL.xyz = tangentL;
#endif
	float4 posW = mul(float4(vin.PosL, 1.0f), world);
	vout.PosW = mul(float4(vin.PosL, 1.0f), world).xyz;
	vout.PosH = mul(float4(vout.PosW, 1.0f), gViewProj);
	
	vout.NormalW = mul(vin.NormalL, (float3x3)world);
	vout.TangentW = mul(vin.TangentL, (float3x3)world);
	vout.TexC = mul(mul(float4(vin.TexC, 0.0f, 1.0f), texTransform), matData.MatTransform).xy;
	
	// Generate projective tex-coords to project shadow map onto scene.
	//vout.ShadowPosH = mul(posW, gShadowTransform);
	return vout;
}

MRT_VSOutput PS(PSInput input)
{
	MaterialData matData = gMaterialData[input.MatIndex];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;
	float3 fresnelR0 = matData.FresnelR0;
	float roughness = matData.Roughness;
	uint diffuseTexIndex = matData.DiffuseMapIndex;
	uint normalTexIndex = matData.NormalMapIndex;


	diffuseAlbedo *= gDiffuseMap[diffuseTexIndex].Sample(gsamAnisotropicWrap, input.TexC);

	input.NormalW = normalize(input.NormalW);
	input.TangentW = normalize(input.TangentW);

	float3 toEyeW = normalize(gEyePosW - input.PosW);

	const float shininess = 1.0f - roughness;
	Material mat = { diffuseAlbedo, fresnelR0, shininess };

	MRT_VSOutput result;
	result.Diffuse = diffuseAlbedo;
	result.Normal = float4(input.NormalW, 1);

	if (normalTexIndex != -1)
	{
		float3 texNormal = gDiffuseMap[normalTexIndex].Sample(gsamAnisotropicWrap, input.TexC);
		float3 normalT = texNormal * 2.0f - 1.0f;

		float3 N = input.NormalW;
		float3 T = normalize(input.TangentW - dot(input.TangentW, N) * N);
		float3 B = cross(N, T);

		float3x3 TBN = float3x3(T, B, N);

		result.Normal = float4(mul(normalT, TBN), 1);
	}

	return result;
}