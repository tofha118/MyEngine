#ifndef _DEFAULT_FX_
#define _DEFAULT_FX_

#include "params.fx"
#include "utils.fx"



struct VS_IN
{
    float3 pos : POSITION;
 //   float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    //가중치
    float4 weight : WEIGHT;
    float4 indices : INDICES;
    //좌표랑 인스턴스 아이디
    row_major matrix matWorld : W;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;
    uint instanceID : SV_InstanceID;
};

struct VS_OUT
{
    float4 pos : SV_Position;
 //   float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;  //view좌표계에서 포지션
    float3 viewNormal : NORMAL; //view좌표계에서 노말
    float3 viewTangent : TANGENT;
    float3 viewBinormal : BINORMAL;

};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

  //  output.pos = float4(input.pos, 1.f);  //앞에거 받은 그대로 넘겨주고 끝에거만 1로 채워라.
  //  output.color = input.color;
    if (g_int_0 == 1)  //이거 1이면 인스턴싱
    {
        //이거 키면 Skinning 들어감
        if (g_int_1 == 1)
            Skinning(input.pos, input.normal, input.tangent, input.weight, input.indices);

        output.pos = mul(float4(input.pos, 1.f), input.matWVP);
        output.uv = input.uv;

        output.viewPos = mul(float4(input.pos, 1.f), input.matWV).xyz;
        output.viewNormal = normalize(mul(float4(input.normal, 0.f), input.matWV).xyz);
        output.viewTangent = normalize(mul(float4(input.tangent, 0.f), input.matWV).xyz);
        output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
    }
    else  //아니면 원래 하던대로
    {
        if (g_int_1 == 1)
            Skinning(input.pos, input.normal, input.tangent, input.weight, input.indices);


        output.pos = mul(float4(input.pos, 1.f), g_matWVP);  //matWVP행렬을 곱해준다. 마지막에  1.f1로 한거는 행렬을 곱할때 1로 세팅하면 그게 좌표, 좌표가 아닌 방향성을 추출하고 싶으면 0으로 확장한다.
        output.uv = input.uv;

        output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;
        output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);
        output.viewTangent = normalize(mul(float4(input.tangent, 0.f), g_matWV).xyz);  //뷰 스페이스를 기준으로 한 노말과 탄젠트를 구함
        output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
        // output.color += offset1;
    }
   
    return output;
}

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};

PS_OUT PS_Main(VS_OUT input)
{
    PS_OUT output = (PS_OUT)0;

    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_on_0==1)
        color = g_tex_0.Sample(g_sam_0, input.uv);

    float3 viewNormal = input.viewNormal;
    if (g_tex_on_1==1)
    {
        // [0,255] 범위에서 [0,1]로 변환
        float3 tangentSpaceNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
        // [0,1] 범위에서 [-1,1]로 변환
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
        float3x3 matTBN = { input.viewTangent, input.viewBinormal, input.viewNormal };
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN));
    }

    //저장하고 뱉어주는거임.  라이트는 계산안할거임.
    output.position = float4(input.viewPos.xyz, 0.f);  //위치
    output.normal = float4(viewNormal.xyz, 0.f); //노말
    output.color = color;  //컬러

    return output;
}
#endif