#include "common.hlsl"

cbuffer LightDirection : register(b1)
{
	float3	Strength;
	float	FalloffStart;
	float3	Direction;
	float	FalloffEnd;
	float3	Position;
	float	SpotPower;
	uint	Type;
	uint3	Pad;
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

	float depth = gBufferMap[0].Load(location3).r;
	Out.LinearDepth = gProj[3][2] / (depth - gProj[2][2]);

	Out.Color = gBufferMap[1].Load(location3).xyz;
	Out.Normal = gBufferMap[2].Load(location3).xyz;

	return Out;
}

float3 CalcWorldPos(float2 csPos, float linearDepth)
{
	float2 values = float2(1 / gProj[0][0], 1 / gProj[1][1]);
	float4 position = float4(csPos.xy * values.xy * linearDepth, linearDepth, 1);

	return mul(position, gInvView).xyz;
}

static const float2 arrPos[4] = {
	float2(-1.0,  1.0),
	float2(1.0,  1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

PSInput VS(uint nVertexID : SV_VertexID)
{
	PSInput vout;

	if(Type == 0)
		vout.PosH = float4(arrPos[nVertexID], 0, 1);
		vout.TexC = vout.PosH.xy;

	return vout;
}

float4 pointVS() : SV_Position
{
	return float4(0.0 ,0.0, 0.0, 1.0);
}

struct HS_CONSTANT_DATA_OUTPUT
{
	float Edges[4] : SV_TessFactor;
	float Inside[2] : SV_InsideTessFactor;
};

HS_CONSTANT_DATA_OUTPUT ConstantHS()
{
	HS_CONSTANT_DATA_OUTPUT output;

	float tessFactor = 18.0;
	output.Edges[0] = output.Edges[1] = output.Edges[2] = output.Edges[3] = tessFactor;
	output.Inside[0] = output.Inside[1] = tessFactor;

	return output;
}

struct HS_OUTPUT
{
	float3 HemilDir : POSITION;
};

static const float3 HemilDir[2] = {
	float3(1.0, 1.0, 1.0),
	float3(-1.0, 1.0, -1.0)
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")] 
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
HS_OUTPUT HS(uint PatchID : SV_PrimitiveID)
{
	HS_OUTPUT output;

	output.HemilDir = HemilDir[PatchID];

	return output;
}

[domain("quad")]
PSInput DS(HS_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DomainLocation, const OutputPatch<HS_OUTPUT, 4> quad)
{
	float2 posClipSpace = UV.xy * 2.0 - 1.0;
	float2 posClipSpaceAbs = abs(posClipSpace.xy);
	float maxLen = max(posClipSpaceAbs.x, posClipSpaceAbs.y);

	float3 normDir = normalize(float3(posClipSpace.xy, maxLen - 1.0) * quad[0].HemilDir);
	float4 posLS = float4(normDir.xyz, 1.0);

	float range = FalloffEnd;
	float4x4 M = transpose(gView);
	M[0] = M[0] * float4((float3)range, 0);
	M[1] = M[1] * float4((float3)range, 0);
	M[2] = -M[2] * float4((float3)range, 0);
	M[3] = float4(Position, 1);

	PSInput output;

	output.PosH = mul(mul(mul(posLS, M), gView), gProj);
	output.TexC = output.PosH.xy / output.PosH.w;

	return output;
}

MRT_VSOutput PS(PSInput input)
{
	float3 L = Direction;
	float3 S = Strength;

	SURFACE_DATA gbd = UnpackGBuffer(input.PosH);

	float3 position = CalcWorldPos(input.TexC, gbd.LinearDepth);

	float d;
	if (Type == 1)
	{
		L = position - Position;
		d = length(L);

		L /= d;
	}
	// diffuse light
	float ndotl = saturate(dot(-L, gbd.Normal));
	float4 diffuse = float4(S, 1) * ndotl;

	// specular light
	float3 toEyeW = normalize(gEyePosW - position);
	float3 halfWay = normalize(toEyeW + -L);
	float ndoth = saturate(dot(halfWay, gbd.Normal));
	float4 specular = float4(S, 1) * pow(ndoth, 5) * 0.2;

	int3 location = int3(input.PosH.xy, 0);

	// °¨¼â
	if (Type == 1)
	{
		float att = saturate((FalloffEnd - d) / FalloffEnd);//1.0 - saturate(dot(d, FalloffEnd));
		//float attn = att * att;
		diffuse *= att;
		specular *= att;
	}

	MRT_VSOutput result;
	result.LDiffuse = float4(gBufferMap[3].Load(location).rgb, 1.0f) + diffuse;
	result.Specular = float4(gBufferMap[4].Load(location).rgb, 1.0f) + specular;

	return result;
}