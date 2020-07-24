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

cbuffer ShadowTransform : register(b2)
{
	float4x4 gLightShadowTransform;
};

struct PSInput
{
	float4 PosH		: SV_POSITION;
	float2 TexC		: TEXCOORD;
};

struct MRT_VSOutput
{
	float4 Diffuse : SV_TARGET0;
	float4 Specular : SV_TARGET1;
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

#define CylinderPortion 0.2
#define ExpendAmount (1.0 + CylinderPortion)

[domain("quad")]
PSInput spotDS(HS_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DomainLocation, const OutputPatch<HS_OUTPUT, 4> quad)
{
	float2 posClipSpace = UV.xy * float2(2.0, -2.0) + float2(-1.0, 1.0);

	float2 posClipSpaceAbs = abs(posClipSpace.xy);
	float maxLen = max(posClipSpaceAbs.x, posClipSpaceAbs.y);

	float2 posClipSpaceNoCylAbs = saturate(posClipSpaceAbs * ExpendAmount);
	float maxLenNoCapsule = max(posClipSpaceNoCylAbs.x, posClipSpaceNoCylAbs.y);
	float2 posClipSpaceNoCyl = sign(posClipSpace.xy) * posClipSpaceNoCylAbs;

	float SinAngle = 3.141592 * 0.25;
	float CosAngle = 3.141592 * 0.25;
	float3 halfSpherePos = normalize(float3(posClipSpaceNoCyl.xy, 1.0 - maxLenNoCapsule));
	halfSpherePos = normalize(float3(halfSpherePos.xy * SinAngle, CosAngle));
	float cylinderOffsetZ = saturate((maxLen * ExpendAmount - 1.0) / CylinderPortion);

	//float3 normDir = normalize(float3(posClipSpace.xy, maxLen - 1.0) * quad[0].HemilDir);
	//float4 posLS = float4(normDir.xyz, 1.0);
	float4 posLS = float4(halfSpherePos.xy * (1.0 - cylinderOffsetZ), halfSpherePos.z - cylinderOffsetZ * CosAngle, 1.0);

	float range = FalloffEnd;
	float4x4 M;
	M[0] = float4(range, 0, 0, 0);
	M[1] = float4(0, 0, range, 0);
	M[2] = float4(0, -range, 0, 0);
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
	if (Type == 2)
	{
		float2 uv = input.PosH.xy / float2(1280, 720) * float2(2, -2) - float2(1, -1);
		input.TexC = uv;
		position = CalcWorldPos(input.TexC, gbd.LinearDepth);
	}

	float d;
	if (Type == 1 || Type == 2)
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
	if (Type == 1 || Type == 2)
	{
		float conAtt = 1;
		if (Type == 2)
		{
			float cosAng = dot(-Direction, -L);
			float outer = cos(3.141592 * 0.25);
			float inner = cos(3.141592 * 0.0);
			conAtt = saturate((cosAng - outer) / (inner - outer));

			//conAtt *= conAtt;
			//conAtt = cosAng;
		}

		float att = 1 - saturate(d / FalloffEnd);
		float attn = att * att;

		att *= conAtt;

		//position /= float3(1080, 256, 1080);

		diffuse *= att;
		specular *= att;
	}

	float shadowFactor = 1;
	if (gLightShadowTransform[3][3])
	{
		float4 shadowPosH = mul(float4(position, 1), gLightShadowTransform);
		shadowFactor = CalcShadowFactor(shadowPosH);
	}

	MRT_VSOutput result;
	result.Diffuse = diffuse * shadowFactor;
	result.Specular = specular * shadowFactor;

	return result;
}