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


struct VS_INSTANCING_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_INSTANCING_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};


VS_INSTANCING_OUTPUT VSInstancing(VS_INSTANCING_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_INSTANCING_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), instance[nInstanceID].transform), gmtxView), gmtxProjection);
	output.color = instance[nInstanceID].color;

	return output;
}

float4 PSInstancing(VS_INSTANCING_OUTPUT input) : SV_TARGET
{
	return input.color;
}