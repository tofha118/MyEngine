#ifndef _SHADOW_FX_
#define _SHADOW_FX_

#include "params.fx"

struct VS_IN
{
    float3 pos : POSITION;
};

struct VS_OUT
{
    float4 pos : SV_Position; //이거는 픽셀 좌표가 됨
    float4 clipPos : POSITION;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0.f;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.clipPos = output.pos;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    return float4(input.clipPos.z / input.clipPos.w, 0.f, 0.f, 0.f);  //투영 좌표계를 기준으로 한 z좌표를 SV_Target에 뱉어
}

#endif