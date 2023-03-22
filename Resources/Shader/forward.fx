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
    float3 viewPos : POSITION;  //view��ǥ�迡�� ������
    float3 viewNormal : NORMAL; //view��ǥ�迡�� �븻
    float3 viewTangent : TANGENT;
    float3 viewBinormal : BINORMAL;

};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

  //  output.pos = float4(input.pos, 1.f);  //�տ��� ���� �״�� �Ѱ��ְ� �����Ÿ� 1�� ä����.
  //  output.color = input.color;
    output.pos = mul(float4(input.pos, 1.f), g_matWVP);  //matWVP����� �����ش�. ��������  1.f1�� �ѰŴ� ����� ���Ҷ� 1�� �����ϸ� �װ� ��ǥ, ��ǥ�� �ƴ� ���⼺�� �����ϰ� ������ 0���� Ȯ���Ѵ�.
    output.uv = input.uv;

    output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;  
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);
    output.viewTangent = normalize(mul(float4(input.tangent, 0.f), g_matWV).xyz);  //�� �����̽��� �������� �� �븻�� ź��Ʈ�� ����
    output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
   // output.color += offset1;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{

     float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_on_0)
        color = g_tex_0.Sample(g_sam_0, input.uv);

    float3 viewNormal = input.viewNormal;  //�ؽ��Ŀ��� �Է¹��� �븻���� �� ��ǥ��� ��ȯ���Ѽ� �Ѱܾ߰���?

    if (g_tex_on_1)
    {
        // [0,255] �������� [0,1]�� ��ȯ
        float3 tangentSpaceNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
        // [0,1] �������� [-1,1]�� ��ȯ
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
        float3x3 matTBN = { input.viewTangent, input.viewBinormal, input.viewNormal };
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN));
    }


       LightColor totalColor = (LightColor)0.f;

       for (int i = 0; i < g_lightCount; ++i)  //��� ����Ʈ�� ��ȸ�ϸ鼭 ������ ���� ���� ������ �ϰ�����
       {
           LightColor color = CalculateLightColor(i, viewNormal, input.viewPos);
           totalColor.diffuse += color.diffuse;
           totalColor.ambient += color.ambient;
           totalColor.specular += color.specular;
       }

       //���� ���� ��ȯ�Ǵ� �κ�
       //���� ������ ������ ���� ���� ���� �������� �̻ڰ� ����� �嶯�Ӥ���
       color.xyz = (totalColor.diffuse.xyz * color.xyz)
           + totalColor.ambient.xyz * color.xyz
           + totalColor.specular.xyz;

    // 
  //  return input.color;  //�׳� �޾ƿ� �÷�
    return color;  //�ؽ��� ����
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