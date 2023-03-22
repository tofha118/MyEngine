#ifndef _UTILS_FX_
#define _UTILS_FX_

#include "params.fx"

//��ü���� rgb���� ������ �ű⿡ �������� ���� ������ ��������
LightColor CalculateLightColor(int lightIndex, float3 viewNormal, float3 viewPos)
{
    LightColor color = (LightColor)0.f;

    float3 viewLightDir = (float3)0.f;

    float diffuseRatio = 0.f;
    float specularRatio = 0.f;
    float distanceRatio = 1.f;
    //����Ʈ Ÿ�Կ� ���� �з�
    if (g_light[lightIndex].lightType == 0)
    {
        // Directional Light
        //��ǻ�� ���� ���� ���
        viewLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);  //ī�޶� �����̽��� �� �����̽��� �������� ��ȯ ���Ѽ� ���.
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));                                        //����ϴ� �� ��ǥ�� �� �ϳ��� �����ؾ���
    //�ڽ��ΰ��� �������
    }
    else if (g_light[lightIndex].lightType == 1)
    {
        // Point Light
        float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
        viewLightDir = normalize(viewPos - viewLightPos);
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));

        float dist = distance(viewPos, viewLightPos);
        if (g_light[lightIndex].range == 0.f)
            distanceRatio = 0.f;
        else
            distanceRatio = saturate(1.f - pow(dist / g_light[lightIndex].range, 2));
    }
    else
    {
        // Spot Light
        float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
        viewLightDir = normalize(viewPos - viewLightPos);
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));

        if (g_light[lightIndex].range == 0.f)
            distanceRatio = 0.f;
        else
        {
            float halfAngle = g_light[lightIndex].angle / 2;

            float3 viewLightVec = viewPos - viewLightPos;
            float3 viewCenterLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);  //���� ������ ���⺤��

            float centerDist = dot(viewLightVec, viewCenterLightDir);
            distanceRatio = saturate(1.f - centerDist / g_light[lightIndex].range);

            float lightAngle = acos(dot(normalize(viewLightVec), viewCenterLightDir));

            if (centerDist < 0.f || centerDist > g_light[lightIndex].range) // �ִ� �Ÿ��� �������
                distanceRatio = 0.f;
            else if (lightAngle > halfAngle) // �ִ� �þ߰��� �������
                distanceRatio = 0.f;
            else // �Ÿ��� ���� ������ ���⸦ ����
                distanceRatio = saturate(1.f - pow(centerDist / g_light[lightIndex].range, 2));
        }
    }

    float3 reflectionDir = normalize(viewLightDir + 2 * (saturate(dot(-viewLightDir, viewNormal)) * viewNormal));
    float3 eyeDir = normalize(viewPos);
    specularRatio = saturate(dot(-eyeDir, reflectionDir));
    specularRatio = pow(specularRatio, 2);  //����� ��� (���� �׶��̼�)

    //���� �����ִ� ���⿡�� �ڽ��ΰ���ŭ ������.(���� ������ �޴� �κ��� ����ϱ� ����)
    color.diffuse = g_light[lightIndex].color.diffuse * diffuseRatio * distanceRatio;  //��ǻ��ϱ�
    color.ambient = g_light[lightIndex].color.ambient * distanceRatio;  //ambient�� �״��
    color.specular = g_light[lightIndex].color.specular * specularRatio * distanceRatio;  //specular

    return color;
}


float Rand(float2 co)  //�ϴ� �� �ٵ� ���� ���� ���� ������
{
    return 0.5 + (frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453)) * 0.5;
}


float CalculateTessLevel(float3 cameraWorldPos, float3 patchPos, float min, float max, float maxLv)
{
    float distance = length(patchPos - cameraWorldPos);  //ī�޶�� ��ġ�� �Ÿ� 

    if (distance < min)
        return maxLv;
    if (distance > max)
        return 1.f;

    float ratio = (distance - min) / (max - min);  //�߾��̸� ������ �̿��� ���� �����.
    float level = (maxLv - 1.f) * (1.f - ratio);
    return level;
}


struct SkinningInfo
{
    float3 pos;
    float3 normal;
    float3 tangent;
};

void Skinning(inout float3 pos, inout float3 normal, inout float3 tangent,
    inout float4 weight, inout float4 indices)
{
    SkinningInfo info = (SkinningInfo)0.f;

    for (int i = 0; i < 4; ++i)
    {
        if (weight[i] == 0.f)
            continue;

        int boneIdx = indices[i];
        matrix matBone = g_mat_bone[boneIdx];

        //����ġ �����ֱ�
        info.pos += (mul(float4(pos, 1.f), matBone) * weight[i]).xyz;
        info.normal += (mul(float4(normal, 0.f), matBone) * weight[i]).xyz;
        info.tangent += (mul(float4(tangent, 0.f), matBone) * weight[i]).xyz;
    }
    //���� ź��Ʈ �븻 ����
    pos = info.pos;
    tangent = normalize(info.tangent);
    normal = normalize(info.normal);
}


#endif