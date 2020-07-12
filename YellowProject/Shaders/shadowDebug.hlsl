#include "common.hlsl"

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float2 TexC    : TEXCOORD;
    uint InstID    : INSTANCE;
};

VertexOut VS(uint nVertexID : SV_VertexID, uint nInstanceID : SV_InstanceID)
{
	VertexOut vout = (VertexOut)0.0f;

    if (nVertexID == 0) vout.PosH = float4(-1.0, 0.5 - (nInstanceID) * 0.5, 0.0, 1.0);
    if (nVertexID == 1) vout.PosH = float4(-1.0, 1.0 - (nInstanceID) * 0.5, 0.0, 1.0);
    if (nVertexID == 2) vout.PosH = float4(-0.5, 0.5 - (nInstanceID) * 0.5, 0.0, 1.0);
    if (nVertexID == 3) vout.PosH = float4(-0.5, 1.0 - (nInstanceID) * 0.5, 0.0, 1.0);

    if (nVertexID == 0) vout.TexC = float2(0, 1);
    if (nVertexID == 1) vout.TexC = float2(0, 0);
    if (nVertexID == 2) vout.TexC = float2(1, 1);
    if (nVertexID == 3) vout.TexC = float2(1, 0);

    vout.InstID = nInstanceID;
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    if (pin.InstID == 0)
    {
        int3 location3 = int3(pin.TexC.x * 1280, pin.TexC.y * 720, 0);
        //float depth = gDiffuseMap[pin.InstID].Load(location3).x;
        float depth = gDiffuseMap[pin.InstID].Sample(gsamLinearWrap, pin.TexC).r;
        float lDepth = (gProj[3][2] / (depth - gProj[2][2])) / 1000;
        return float4(lDepth.rrr, 1);
    }
    if(pin.InstID == 1)
        return float4(gDiffuseMap[pin.InstID].Sample(gsamLinearWrap, pin.TexC).rgb, 1.0f);
    if(pin.InstID == 2)
        return float4(gDiffuseMap[pin.InstID].Sample(gsamLinearWrap, pin.TexC).rgb, 1.0f);
    return float4(0, 0, 0, 1);
}


