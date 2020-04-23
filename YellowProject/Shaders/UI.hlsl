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
	InstanceData instData = gInstanceData[instanceID];

	PSInput vout;
	vout.MatIndex = gMaterialIndexData[instanceID * instData.MaterialIndexStride].MaterialIndex;
	vout.PosH = mul(float4(vin.PosL.xy, 0, 1), gInstanceData[instanceID].World);
	vout.TexC = vin.TexC;

	return vout;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	uint diffuseTexIndex = gMaterialData[input.MatIndex].DiffuseMapIndex;

	return gDiffuseMap[diffuseTexIndex].Sample(gsamAnisotropicWrap, input.TexC);
}