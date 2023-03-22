#ifndef _PARTICLE_FX_
#define _PARTICLE_FX_

#include "params.fx"
#include "utils.fx"

struct Particle
{
    float3  worldPos; //��ġ
    float   curTime;  //��� �ð�
    float3  worldDir;  //����
    float   lifeTime;  //�󸶳� ���������
    int     alive;  //�׾��� ��ҳ� (1�̸� ��Ҵ�
    float3  padding;  //���߱��ڵ�
};

StructuredBuffer<Particle> g_data : register(t9);

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    uint id : SV_InstanceID;
};

struct VS_OUT
{
    float4 viewPos : POSITION;
    float2 uv : TEXCOORD;
    float id : ID;
};

// VS_MAIN
// g_float_0    : Start Scale
// g_float_1    : End Scale
// g_tex_0      : Particle Texture

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0.f;
    //������ �ش��ϴ� ���� ã�Ƽ� ��ǥ ��� ���ִ�����
    float3 worldPos = mul(float4(input.pos, 1.f), g_matWorld).xyz;
    worldPos += g_data[input.id].worldPos;

    output.viewPos = mul(float4(worldPos, 1.f), g_matView);
    output.uv = input.uv;
    output.id = input.id;

    return output;
}
//���� ������Ʈ���� ����Ұ���. ���� �߰������� �Ұ���
struct GS_OUT
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
    uint id : SV_InstanceID;
};

[maxvertexcount(6)]  //�ϴ� �ﰢ���� �ƴ϶� �� �ϳ��� �Ѱ��ְ� �װŸ� ������Ʈ�� �ܰ迡�� ����ִ°���.
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)  //�׷����°Ÿ� ���� ����ְ� �ƴϸ� �Ѱ�
{
    GS_OUT output[4] =
    {
        (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f
    };

    VS_OUT vtx = input[0];
    uint id = (uint)vtx.id;
    if (0 == g_data[id].alive)  //����ִ°� �ƴϸ� �����ؼ� �� �ϳ��� �ȱ׷���
        return;

    //�׷����°Ÿ� ���� ��ü �׷���
    float ratio = g_data[id].curTime / g_data[id].lifeTime;
    float scale = ((g_float_1 - g_float_0) * ratio + g_float_0) / 2.f;

    // View Space
    output[0].position = vtx.viewPos + float4(-scale, scale, 0.f, 0.f);
    output[1].position = vtx.viewPos + float4(scale, scale, 0.f, 0.f);
    output[2].position = vtx.viewPos + float4(scale, -scale, 0.f, 0.f);
    output[3].position = vtx.viewPos + float4(-scale, -scale, 0.f, 0.f);

    // Projection Space  ��������� �ٽ� �ϰ�
    output[0].position = mul(output[0].position, g_matProjection);
    output[1].position = mul(output[1].position, g_matProjection);
    output[2].position = mul(output[2].position, g_matProjection);
    output[3].position = mul(output[3].position, g_matProjection);
    //uv����
    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);
    //id����
    output[0].id = id;
    output[1].id = id;
    output[2].id = id;
    output[3].id = id;
    //�ﰢ�� 2���� �־��ִ��� �׸��� ������ ���.
    outputStream.Append(output[0]);
    outputStream.Append(output[1]);
    outputStream.Append(output[2]);
    outputStream.RestartStrip();

    outputStream.Append(output[0]);
    outputStream.Append(output[2]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();
}

float4 PS_Main(GS_OUT input) : SV_Target
{
    return g_tex_0.Sample(g_sam_0, input.uv);
}

struct ComputeShared
{
    int addCount;
    float3 padding;
};

RWStructuredBuffer<Particle> g_particle : register(u0); //1���� �迭
RWStructuredBuffer<ComputeShared> g_shared : register(u1);  //�������� Ȱ���� ����

// CS_Main
// g_vec2_1 : DeltaTime / AccTime  �ð�
// g_int_0  : Particle Max Count
// g_int_1  : AddCount
// g_vec4_0 : MinLifeTime / MaxLifeTime / MinSpeed / MaxSpeed  �����ϰ� ���̷���
[numthreads(1024, 1, 1)] //������ �׷����� ����� ����� �޸𸮸� ����Ҽ��� �ְ� ����ȭ�� ����
void CS_Main(int3 threadIndex : SV_DispatchThreadID)  //��� ���� �ؾ� �Ұ� ������ ��������� ����. �׷��� ���� �ε����� ���ǰŸ� �ǵ帰�ٰų� ���ļ� �Ѵٸ� ����... ��ġ�� ��찡 �������..?
{
    if (threadIndex.x >= g_int_0)
        return;

    int maxCount = g_int_0;
    int addCount = g_int_1;
    int frameNumber = g_int_2;
    float deltaTime = g_vec2_1.x;
    float accTime = g_vec2_1.y;
    float minLifeTime = g_vec4_0.x;
    float maxLifeTime = g_vec4_0.y;
    float minSpeed = g_vec4_0.z;
    float maxSpeed = g_vec4_0.w;

    g_shared[0].addCount = addCount;  //������۸� �����ִ� �κ��� �׷��� ����ó���� �Ͼ���־ ����
    GroupMemoryBarrierWithGroupSync();  //�׷��� ������ ���༭ ������ �������� ���ִ°���. (����ȭ�ϴ°�)

    if (g_particle[threadIndex.x].alive == 0)
    {
        while (true)
        {
            int remaining = g_shared[0].addCount;
            if (remaining <= 0)  //���⼭�� ���� ��Ȱ�� �������� �� ������ ��. 
                break;

            int expected = remaining;
            int desired = remaining - 1;
            int originalValue;
            InterlockedCompareExchange(g_shared[0].addCount, expected, desired, originalValue); //addcount==expected�� ������ desired���� addcount��, �׸��� addcount�� ������ ���ڰ����� 
            //���� ���� �̷��� �Ҷ� InterlockedCompareExchange �̷� �ڵ带 ���� ����
            if (originalValue == expected)  //�̰� ���ٴ°Ŵ� ���� ���������� ���Դٴ°���.
            {
                g_particle[threadIndex.x].alive = 1;  //�¸������� �� ��Ȱ
                break;
            }
        }

        //����� �Դٴ°Ŵ� ���� ���� ��Ȱ�Ѵٴ¶���
        //���� �����ϰ� ������ ����� �ϴµ� �ƽ��Ե� hlsl���� �����Լ��� ����...
        if (g_particle[threadIndex.x].alive == 1)
        {
            //�ϴ� �� �����ϰ� ���ַ��°���...
            //������ �ؽ��ĸ� �����ͼ� �ű��� uv�� �����ؼ� �������ִ� ������ ������ �����ϴ� ��ĵ� ����.
            //�ٵ� �ϴ� �׳�.. �����̸� �Ǵ°Ŵϱ�.. �����ؽ� �ϴ�
            float x = ((float)threadIndex.x / (float)maxCount) + accTime; //����� �ð��� ��� �����ִ°Ÿ� ���� �õ尪���� ����ϴ�����

            float r1 = Rand(float2(x, accTime));
            float r2 = Rand(float2(x * accTime, accTime));
            float r3 = Rand(float2(x * accTime * accTime, accTime * accTime));

            // [0.5~1] -> [0~1]
            float3 noise =
            {
                2 * r1 - 1,
                2 * r2 - 1,
                2 * r3 - 1
            };

            // [0~1] -> [-1~1]
            float3 dir = (noise - 0.5f) * 2.f;
            //�ʱ�ȭ
            //�������ʹ� �� �����͸� �ǵ�°Ŵϱ� ����ȭ ����� �ʿ䰡 ����
            g_particle[threadIndex.x].worldDir = normalize(dir);
            g_particle[threadIndex.x].worldPos = (noise.xyz - 0.5f) * 25;
            g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * noise.x) + minLifeTime;
            g_particle[threadIndex.x].curTime = 0.f; 
        }
    }
    else
    {
        g_particle[threadIndex.x].curTime += deltaTime;
        if (g_particle[threadIndex.x].lifeTime < g_particle[threadIndex.x].curTime)
        {
            g_particle[threadIndex.x].alive = 0;
            return;
        }

        float ratio = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;
        float speed = (maxSpeed - minSpeed) * ratio + minSpeed;
        g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].worldDir * speed * deltaTime;
    }
}

#endif