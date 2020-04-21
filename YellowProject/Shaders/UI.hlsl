#include "shaders\\Common.hlsl"

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

PSInput VSMain(VSInput vin, uint instanceID : SV_InstanceID)
{
	PSInput vout;

	InstanceData instData = gInstanceData[instanceID];

	uint matIndex = gMaterialIndexData[instanceID * instData.MaterialIndexStride].MaterialIndex;


	float2 pos = (vin.PosL.xy * gInstanceData[instanceID].World._11_22 + gInstanceData[instanceID].World._41_42) / gRenderTargetSize * 2 - 1;

	vout.MatIndex = matIndex;
	vout.PosH = float4(pos, 0, 1);
	vout.TexC = vin.TexC;

	return vout;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	MaterialData matData = gMaterialData[input.MatIndex];
	uint diffuseTexIndex = matData.DiffuseMapIndex;

	return gDiffuseMap[diffuseTexIndex].Sample(gsamAnisotropicWrap, input.TexC);
}