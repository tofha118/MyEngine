#ifndef _COMPUTE_FX_
#define _COMPUTE_FX_

#include "params.fx"

RWTexture2D<float4> g_rwtex_0 : register(u0);  //u0 -> compute 쉐이더 전용으로 사용하는 애임 RWTexture2D는 읽고 쓰는게 가능하다는 소리임

// 쓰레드 그룹당 쓰레드 개수
// max : 1024 (CS_5.0)
// - 하나의 쓰레드 그룹은 하나의 다중처리기에서 실행
[numthreads(1024, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    if (threadIndex.y % 2 == 0)  //대충 gpu아이디를 픽셀의 좌표로 사용하겠다.
        g_rwtex_0[threadIndex.xy] = float4(1.f, 0.f, 0.f, 1.f);
    else
        g_rwtex_0[threadIndex.xy] = float4(0.f, 1.f, 0.f, 1.f);
}

#endif