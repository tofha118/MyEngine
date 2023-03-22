#ifndef _UTILS_FX_
#define _UTILS_FX_

#include "params.fx"

//물체마다 rgb값이 있으면 거기에 더해지는 빛의 색상이 무엇인지
LightColor CalculateLightColor(int lightIndex, float3 viewNormal, float3 viewPos)
{
    LightColor color = (LightColor)0.f;

    float3 viewLightDir = (float3)0.f;

    float diffuseRatio = 0.f;
    float specularRatio = 0.f;
    float distanceRatio = 1.f;
    //라이트 타입에 따라 분류
    if (g_light[lightIndex].lightType == 0)
    {
        // Directional Light
        //디퓨즈 관련 공식 사용
        viewLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);  //카메라 스페이스인 뷰 스페이스를 기준으로 변환 시켜서 계산.
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));                                        //계산하는 뷰 좌표는 꼭 하나로 통일해야함
    //코싸인값을 얻기위함
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
            float3 viewCenterLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);  //빛이 나가는 방향벡터

            float centerDist = dot(viewLightVec, viewCenterLightDir);
            distanceRatio = saturate(1.f - centerDist / g_light[lightIndex].range);

            float lightAngle = acos(dot(normalize(viewLightVec), viewCenterLightDir));

            if (centerDist < 0.f || centerDist > g_light[lightIndex].range) // 최대 거리를 벗어났는지
                distanceRatio = 0.f;
            else if (lightAngle > halfAngle) // 최대 시야각을 벗어났는지
                distanceRatio = 0.f;
            else // 거리에 따라 적절히 세기를 조절
                distanceRatio = saturate(1.f - pow(centerDist / g_light[lightIndex].range, 2));
        }
    }

    float3 reflectionDir = normalize(viewLightDir + 2 * (saturate(dot(-viewLightDir, viewNormal)) * viewNormal));
    float3 eyeDir = normalize(viewPos);
    specularRatio = saturate(dot(-eyeDir, reflectionDir));
    specularRatio = pow(specularRatio, 2);  //비쥬얼 요소 (색상 그라데이션)

    //빛이 갖고있던 세기에서 코싸인값만큼 곱해줌.(빛의 영향을 받는 부분을 계산하기 위함)
    color.diffuse = g_light[lightIndex].color.diffuse * diffuseRatio * distanceRatio;  //디퓨즈구하기
    color.ambient = g_light[lightIndex].color.ambient * distanceRatio;  //ambient는 그대로
    color.specular = g_light[lightIndex].color.specular * specularRatio * distanceRatio;  //specular

    return color;
}


float Rand(float2 co)  //일단 뭐 다들 많이 쓰는 랜덤 가져옴
{
    return 0.5 + (frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453)) * 0.5;
}


float CalculateTessLevel(float3 cameraWorldPos, float3 patchPos, float min, float max, float maxLv)
{
    float distance = length(patchPos - cameraWorldPos);  //카메라와 패치의 거리 

    if (distance < min)
        return maxLv;
    if (distance > max)
        return 1.f;

    float ratio = (distance - min) / (max - min);  //중앙이면 비율을 이용해 레벨 뱉어줌.
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

        //가중치 곱해주기
        info.pos += (mul(float4(pos, 1.f), matBone) * weight[i]).xyz;
        info.normal += (mul(float4(normal, 0.f), matBone) * weight[i]).xyz;
        info.tangent += (mul(float4(tangent, 0.f), matBone) * weight[i]).xyz;
    }
    //최종 탄젠트 노말 나옴
    pos = info.pos;
    tangent = normalize(info.tangent);
    normal = normalize(info.normal);
}


#endif