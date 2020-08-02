#include "cbPass.hlsl"
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

    uint index = nInstanceID;
    if (index > 5)
        index += 24;

    int cnt = 6;
    float size = 2.0 / cnt;

    if (nVertexID == 0) vout.PosH = float4(-1.0 +        (index / cnt) * size, 1.0 - size - (index % cnt) * size, 0.0, 1.0);
    if (nVertexID == 1) vout.PosH = float4(-1.0 +        (index / cnt) * size, 1.0 -        (index % cnt) * size, 0.0, 1.0);
    if (nVertexID == 2) vout.PosH = float4(-1.0 + size + (index / cnt) * size, 1.0 - size - (index % cnt) * size, 0.0, 1.0);
    if (nVertexID == 3) vout.PosH = float4(-1.0 + size + (index / cnt) * size, 1.0 -        (index % cnt) * size, 0.0, 1.0);

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
        float depth = gBufferMap[pin.InstID].Sample(gsamLinearWrap, pin.TexC).r;
        float lDepth = (gProj[3][2] / (depth - gProj[2][2])) / 1000;
        return float4(lDepth.rrr, 1);
    }
    if(pin.InstID == 1)
        return float4(gBufferMap[pin.InstID].Sample(gsamLinearWrap, pin.TexC).rgb, 1.0f);
    if(pin.InstID == 2)
        return float4(gBufferMap[pin.InstID].Sample(gsamLinearWrap, pin.TexC).rgb, 1.0f);
    if(pin.InstID == 3)
        return float4(gBufferMap[pin.InstID].Sample(gsamLinearWrap, pin.TexC).rgb, 1.0f);
    if(pin.InstID == 4)
        return float4(gBufferMap[pin.InstID].Sample(gsamLinearWrap, pin.TexC).rgb, 1.0f);
    if (pin.InstID == 5)
    {
        float distance = gBufferMap[1].Sample(gsamLinearWrap, pin.TexC).w;
        if (distance < 10) return float4(0.02, 0.62, 0.75, 1);
        if (distance < 50) return float4(0.70, 0.75, 0.31, 1);
        if (distance < 100) return float4(0.95, 0.78, 0.29, 1);
        if (distance < 200) return float4(0.95, 0.81, 0.82, 1);
        return float4(1, 1, 1, 1);
    }
    if(pin.InstID > 5)
        return float4(gShadowMap[pin.InstID-6].Sample(gsamLinearWrap, pin.TexC).rrr, 1.0f);
    return float4(0, 0, 0, 1);
}
