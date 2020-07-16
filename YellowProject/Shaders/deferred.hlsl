#include "common.hlsl"

struct PSInput
{
	float4 PosH		: SV_POSITION;
	float2 TexC		: TEXCOORD;
};

static const float2 arrPos[4] = {
	float2(-1.0,  1.0),
	float2( 1.0,  1.0),
	float2(-1.0, -1.0),
	float2( 1.0, -1.0),
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
	float2 values = float2(1/gProj[0][0], 1/gProj[1][1]);
	float4 position = float4(csPos.xy * values.xy * linearDepth, linearDepth, 1);

	return mul(position, gInvView).xyz;
}

float4 PS(PSInput input) : SV_TARGET
{
	SURFACE_DATA gbd = UnpackGBuffer(input.PosH);
	if (length(gbd.Normal) == 0)
		return float4(gbd.Color, 1);

	int3 location = int3(input.PosH.xy, 0);
	float3 LDiffuse = gDiffuseMap[3].Load(location).rgb;
	float3 Specular = gDiffuseMap[4].Load(location).rgb;

	float3 ambient = gAmbientLight * gbd.Color;

	return float4(gbd.Color * (gAmbientLight + LDiffuse + Specular), 1);
}