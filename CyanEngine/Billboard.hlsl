//#define _WITH_BILLBOARD_ANIMATION

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

struct VS_BILLBOARD_INSTANCING_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	//float4 billboardInfo : BILLBOARDINFO; //(cx, cy, type, texture)
};

struct VS_BILLBOARD_INSTANCING_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

//#define _WITH_BILLBOARD_ANIMATION

VS_BILLBOARD_INSTANCING_OUTPUT VSBillboardInstancing(VS_BILLBOARD_INSTANCING_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_BILLBOARD_INSTANCING_OUTPUT output;

	if (input.position.x < 0.0f) input.position.x = -(instance[nInstanceID].color.x * 0.5f);
	else if (input.position.x > 0.0f) input.position.x = (instance[nInstanceID].color.x * 0.5f);
	if (input.position.y < 0.0f) input.position.y = -(instance[nInstanceID].color.y * 0.5f);
	else if (input.position.y > 0.0f) input.position.y = (instance[nInstanceID].color.y * 0.5f);

	float3 position = float3(instance[nInstanceID].transform._41, instance[nInstanceID].transform._42, instance[nInstanceID].transform._43);
	
	float3 f3Look = normalize(gvCameraPosition - position);
	float3 f3Up = float3(0.0f, 1.0f, 0.0f);
	float3 f3Right = normalize(cross(f3Up, f3Look));

	matrix mtxWorld;
	mtxWorld[0] = float4(f3Right, 0.0f);
	mtxWorld[1] = float4(f3Up, 0.0f);
	mtxWorld[2] = float4(f3Look, 0.0f);
	mtxWorld[3] = float4(position, 1.0f);

	output.position = mul(mul(mul(float4(input.position, 1.0f), mtxWorld), gmtxView), gmtxProjection);

//#ifdef _WITH_BILLBOARD_ANIMATION
//	if (input.uv.y < 0.7f)
//	{
//		float fShift = 0.0f;
//		uint nResidual = ((uint)gfCurrentTime % 4);
//		if (nResidual == 1) fShift = -gfElapsedTime * 10.5f;
//		if (nResidual == 3) fShift = +gfElapsedTime * 10.5f;
//		input.uv.x += fShift;
//	}
//#endif
	output.uv = input.uv;

	//output.textureID = (int)input.billboardInfo.w - 1;

	return(output);
}

float4 PSBillboardInstancing(VS_BILLBOARD_INSTANCING_OUTPUT input) : SV_TARGET
{
	//return float4(1, 0, 0, 1);
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);

	return(cColor);
}