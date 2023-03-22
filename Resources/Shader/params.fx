#ifndef _PARAMS_FX_
#define _PARAMS_FX_


//light정보와 같은 정보를 받아오는거는(자료형 맞춰준거는) hlsl에서는 16바이트 단위로 정렬하기 때문임
//이렇게 안하면 엉뚱한 값을 긁어올수도있는상황이 올수도
struct LightColor
{
    float4      diffuse;
    float4      ambient;
    float4      specular;
};

struct LightInfo
{
    LightColor  color;
    float4	    position;
    float4	    direction;
    int		    lightType;
    float	    range;
    float	    angle;
    int  	    padding;  //얘를 넣어주는거는 정렬을 맞춰주기 위함임. 16바이트 딱 떨어지도록
};

cbuffer GLOBAL_PARAMS : register(b0)
{
    int         g_lightCount;
    float3      g_lightPadding;
    LightInfo   g_light[50];
}

cbuffer TRANSFORM_PARAMS : register(b1)
{
    row_major matrix g_matWorld;
    row_major matrix g_matView;
    row_major matrix g_matProjection;
    row_major matrix g_matWV;
    row_major matrix g_matWVP;
    row_major matrix g_matViewInv;

};

//쉐이더는 null체크가 안됌..
cbuffer MATERIAL_PARAMS : register(b2)
{
    int     g_int_0;
    int     g_int_1;
    int     g_int_2;
    int     g_int_3;
    float   g_float_0;
    float   g_float_1;
    float   g_float_2;
    float   g_float_3;
    int     g_tex_on_0;
    int     g_tex_on_1;
    int     g_tex_on_2;
    int     g_tex_on_3;
    float2  g_vec2_0;
    float2  g_vec2_1;
    float2  g_vec2_2;
    float2  g_vec2_3;
    float4  g_vec4_0;
    float4  g_vec4_1;
    float4  g_vec4_2;
    float4  g_vec4_3;
    row_major float4x4 g_mat_0;
    row_major float4x4 g_mat_1;
    row_major float4x4 g_mat_2;
    row_major float4x4 g_mat_3;
};

Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);

StructuredBuffer<Matrix> g_mat_bone : register(t7);  //본 이랑관련된 애로 사용하겠다

SamplerState g_sam_0 : register(s0);

#endif