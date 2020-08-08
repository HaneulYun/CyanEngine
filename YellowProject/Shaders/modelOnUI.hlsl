#include "common.hlsl"
#include "cbPass.hlsl"

struct VSInput
{
	float3 PosL		: POSITION;
	float3 NormalL	: NORMAL;
	float2 TexC		: TEXCOORD;
	float3 TangentL : TANGENT;
};

struct PSInput
{
	float4 PosH		: SV_POSITION;
	float2 TexC		: TEXCOORD;

	nointerpolation uint MatIndex : MATINDEX;
};

PSInput VS(VSInput vin, uint instanceID : SV_InstanceID)
{
	InstanceData instData = gInstanceData[instanceID];
	float4x4 world = instData.World;

	PSInput vout;
	vout.MatIndex = gMaterialIndexData[instanceID * instData.MaterialIndexStride].MaterialIndex;
	vout.PosH = mul(float4(vin.PosL, 1), world);// gInstanceData[instanceID].World);
	vout.TexC = vin.TexC;

	return vout;
}

float4 PS(PSInput input) : SV_TARGET
{
	uint diffuseTexIndex = gMaterialData[input.MatIndex].DiffuseMapIndex;
	float4 albedo = gMaterialData[input.MatIndex].DiffuseAlbedo;

	return gDiffuseMap[diffuseTexIndex].Sample(gsamAnisotropicWrap, input.TexC) * albedo;
}