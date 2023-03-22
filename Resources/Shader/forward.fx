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
    output.pos = mul(float4(input.pos, 1.f), g_matWVP);  //matWVP행렬을 곱해준다. 마지막에  1.f1로 한거는 행렬을 곱할때 1로 세팅하면 그게 좌표, 좌표가 아닌 방향성을 추출하고 싶으면 0으로 확장한다.
    output.uv = input.uv;

    output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;  
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);
    output.viewTangent = normalize(mul(float4(input.tangent, 0.f), g_matWV).xyz);  //뷰 스페이스를 기준으로 한 노말과 탄젠트를 구함
    output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
   // output.color += offset1;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{

     float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_on_0)
        color = g_tex_0.Sample(g_sam_0, input.uv);

    float3 viewNormal = input.viewNormal;  //텍스쳐에서 입력받은 노말값을 뷰 좌표계로 변환시켜서 넘겨야겠찡?

    if (g_tex_on_1)
    {
        // [0,255] 범위에서 [0,1]로 변환
        float3 tangentSpaceNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
        // [0,1] 범위에서 [-1,1]로 변환
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
        float3x3 matTBN = { input.viewTangent, input.viewBinormal, input.viewNormal };
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN));
    }


       LightColor totalColor = (LightColor)0.f;

       for (int i = 0; i < g_lightCount; ++i)  //모든 라이트를 순회하면서 각각의 색상에 대한 연산을 하고있음
       {
           LightColor color = CalculateLightColor(i, viewNormal, input.viewPos);
           totalColor.diffuse += color.diffuse;
           totalColor.ambient += color.ambient;
           totalColor.specular += color.specular;
       }

       //빛에 의해 변환되는 부분
       //딱히 정해진 연산은 없고 대충 적은 연산으로 이쁘게 만들면 장땡임ㅋㅋ
       color.xyz = (totalColor.diffuse.xyz * color.xyz)
           + totalColor.ambient.xyz * color.xyz
           + totalColor.specular.xyz;

    // 
  //  return input.color;  //그냥 받아온 컬러
    return color;  //텍스쳐 색상
}

// [Texture Shader]
// g_tex_0 : Output Texture
// AlphaBlend : true
struct VS_TEX_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_TEX_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VS_TEX_OUT VS_Tex(VS_TEX_IN input)
{
    VS_TEX_OUT output = (VS_TEX_OUT)0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    return output;
}

float4 PS_Tex(VS_TEX_OUT input) : SV_Target
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_on_0)
        color = g_tex_0.Sample(g_sam_0, input.uv);

    return color;
}

#endif