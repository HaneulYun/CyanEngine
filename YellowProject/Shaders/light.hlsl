#include "common.hlsl"

cbuffer LightDirection : register(b0)
{
	float3		Direction;
	float pad0;
	float3		Strength;
	float pad1;
};

struct PSInput
{
	float4 PosH		: SV_POSITION;
	float2 TexC		: TEXCOORD;
};

struct MRT_VSOutput
{
	float4 Color : SV_TARGET0;
	float4 Diffuse : SV_TARGET1;
	float4 Normal : SV_TARGET2;
	float4 LDiffuse : SV_TARGET3;
	float4 Specular : SV_TARGET4;
};

static const float2 arrPos[4] = {
	float2(-1.0,  1.0),
	float2(1.0,  1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

PSInput VS(uint nVertexID : SV_VertexID)
{
	PSInput vout;
	vout.PosH = float4(arrPos[nVertexID], 0, 1);
	vout.TexC = vout.PosH.xy;

	return vout;
}

struct SURFACE_DATA
{
	float LinearDepth;
	float3 Color;
	float3 Normal;
	float SpecInt;
	float SpecPow;
};

SURFACE_DATA UnpackGBuffer(int2 location)
{
	SURFACE_DATA Out;

	int3 location3 = int3(location, 0);

	float depth = gDiffuseMap[0].Load(location3).r;
	Out.LinearDepth = gProj[3][2] / (depth - gProj[2][2]);

	Out.Color = gDiffuseMap[1].Load(location3).xyz;
	Out.Normal = gDiffuseMap[2].Load(location3).xyz;

	return Out;
}

float3 CalcWorldPos(float2 csPos, float linearDepth)
{
	float2 values = float2(1 / gProj[0][0], 1 / gProj[1][1]);
	float4 position = float4(csPos.xy * values.xy * linearDepth, linearDepth, 1);

	return mul(position, gInvView).xyz;
}

MRT_VSOutput PS(PSInput input)
{
	float3 L = Direction;
	float3 S = Strength;

	SURFACE_DATA gbd = UnpackGBuffer(input.PosH);

	float3 position = CalcWorldPos(input.TexC, gbd.LinearDepth);

	// diffuse light
	float ndotl = saturate(dot(-L, gbd.Normal));
	float4 diffuse = float4(S, 1) * ndotl;

	// specular light
	float3 toEyeW = normalize(gEyePosW - position);
	float3 halfWay = normalize(toEyeW + -L);
	float ndoth = saturate(dot(halfWay, gbd.Normal));
	float4 specular = float4(S, 1) * pow(ndoth, 5) * 0.2;

	int3 location = int3(input.PosH.xy, 0);

	MRT_VSOutput result;
	result.LDiffuse = float4(gDiffuseMap[3].Load(location).rgb, 1.0f) + diffuse;
	result.Specular = float4(gDiffuseMap[4].Load(location).rgb, 1.0f) + specular;

	return result;
}