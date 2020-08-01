#include "cbPass.hlsl"
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

struct VertexIn
{
	float3 PosL		: POSITION;
	float3 NormalL	: NORMAL;
	float2 TexC		: TEXCOORD;
	float3 TangentL : TANGENT;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 NormalL	: NORMAL;
	float3 PosL		: POSITION;
};

struct MRT_VSOutput
{
	float4 Diffuse : SV_TARGET0;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
	VertexOut vout;

	vout.PosL = vin.PosL;
	vout.NormalL = vin.NormalL;
	float4 posW = mul(float4(vin.PosL, 1.0f), gInstanceData[instanceID].World);
	posW.xyz += gEyePosW;

	vout.PosH = mul(posW, gViewProj).xyww;

	return vout;
}

MRT_VSOutput PS(VertexOut pin)
{
	MRT_VSOutput result;
	result.Diffuse.xyz = gCubeMap.Sample(gsamLinearWrap, pin.PosL);// *Strength;
	result.Diffuse.w = 1000;

	float3 toLight = normalize(-Direction);
	pin.NormalL = normalize(pin.NormalL);

	float degreeFromHorizon = degrees(asin(dot(float3(0, 1, 0), pin.NormalL)));
	float degreeFromHorizonToSun = degrees(asin(dot(float3(0, 1, 0), toLight)));
	float degreeFromSun = degrees(acos(dot(toLight, pin.NormalL)));


	float innerDegrees[4] = { 5, 3, 2, 1 };
	float outerDegrees[4] = { 7, 9, 5, 1 };
	float3 innerColors[4] = { float3(1, 1, 1), float3(1, 1, 1), float3(0.949, 0.807, 0.105), float3(0.450, 0.007, 0.007) };
	float3 outerColors[4] = { float3(1, 1, 1), float3(0.949, 0.807, 0.105), float3(0.949, 0.607, 0.188), float3(0.450, 0.007, 0.007) };

	float innerDegree;
	float outerDegree;
	float3 innerColor;
	float3 outerColor;

	float3 horizontalColor;;
	float3 weight = 1;

	if (degreeFromHorizonToSun > 30)
	{
		innerDegree = innerDegrees[0];
		outerDegree = outerDegrees[0];

		innerColor = innerColors[0];
		outerColor = outerColors[0];

		outerColor = outerColors[0];
	}
	else if (degreeFromHorizonToSun > 15)
	{
		innerDegree = lerp(innerDegrees[0], innerDegrees[1], (30 - degreeFromHorizonToSun) / 15);
		outerDegree = lerp(outerDegrees[0], outerDegrees[1], (30 - degreeFromHorizonToSun) / 15);

		innerColor = lerp(innerColors[0], innerColors[1], (30 - degreeFromHorizonToSun) / 15);
		outerColor = lerp(outerColors[0], outerColors[1], (30 - degreeFromHorizonToSun) / 15);
	}
	else if (degreeFromHorizonToSun > 0)
	{
		innerDegree = lerp(innerDegrees[1], innerDegrees[2], (15 - degreeFromHorizonToSun) / 15);
		outerDegree = lerp(outerDegrees[1], outerDegrees[2], (15 - degreeFromHorizonToSun) / 15);

		innerColor = lerp(innerColors[1], innerColors[2], (15 - degreeFromHorizonToSun) / 15);
		outerColor = lerp(outerColors[1], outerColors[2], (15 - degreeFromHorizonToSun) / 15);
		outerColor = lerp(outerColors[1], outerColors[2], (15 - degreeFromHorizonToSun) / 15);
		weight = lerp(1.0, 0.7, (15 - degreeFromHorizonToSun) / 15);
	}
	else if (degreeFromHorizonToSun > -30)
	{
		innerDegree = lerp(innerDegrees[2], innerDegrees[3], (-degreeFromHorizonToSun) / 30);
		outerDegree = lerp(outerDegrees[2], outerDegrees[3], (-degreeFromHorizonToSun) / 30);

		innerColor = lerp(innerColors[2], innerColors[3], (-degreeFromHorizonToSun) / 30);
		outerColor = lerp(outerColors[2], outerColors[3], (-degreeFromHorizonToSun) / 30);

		weight = lerp(0.7, 0.1, (-degreeFromHorizonToSun) / 30);
	}
	else
	{
		innerColor = lerp(innerColors[3], float3(0.1, 0.1, 0.1), (-30-degreeFromHorizonToSun) / 60);
		outerColor = lerp(outerColors[3], float3(0.1, 0.1, 0.1), (-30-degreeFromHorizonToSun) / 60);

		weight = 0.1;
	}

	float3 sunriseAmbient;
	if (degreeFromHorizon > 30)
	{
	}
	else if (degreeFromHorizon > 15)
	{
	}
	else if (degreeFromHorizon > 0)
	{
		sunriseAmbient = lerp(result.Diffuse.xyz, outerColor, (15 - degreeFromHorizon) / 15);

		float range;
		if (degreeFromSun < 90)
			range = lerp(1, 0, degreeFromSun / 90);

		result.Diffuse.xyz = lerp(result.Diffuse.xyz, sunriseAmbient, range);
	}
	else if (degreeFromHorizon > -30)
	{
		sunriseAmbient = lerp(outerColor, outerColors[3], (-degreeFromHorizon) / 30);

		float range;
		if(degreeFromSun < 90)
			range = lerp(1, 0, degreeFromSun / 90);

		result.Diffuse.xyz = lerp(result.Diffuse.xyz, sunriseAmbient, range);
	}

	result.Diffuse.xyz *= weight;

	if (degreeFromSun < innerDegree)
		result.Diffuse.xyz = lerp(innerColor, outerColor, degreeFromSun / innerDegree);
	else if (degreeFromSun < outerDegree)
		result.Diffuse.xyz = lerp(outerColor, result.Diffuse.xyz, (degreeFromSun - innerDegree) / (outerDegree - innerDegree));
	
	return result;;
}