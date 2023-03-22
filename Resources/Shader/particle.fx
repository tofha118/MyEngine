#ifndef _PARTICLE_FX_
#define _PARTICLE_FX_

#include "params.fx"
#include "utils.fx"

struct Particle
{
    float3  worldPos; //위치
    float   curTime;  //경과 시간
    float3  worldDir;  //방향
    float   lifeTime;  //얼마나 살아있을지
    int     alive;  //죽었나 살았나 (1이면 살았다
    float3  padding;  //맞추기코드
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
    //나한테 해당하는 아이 찾아서 좌표 계산 해주는중임
    float3 worldPos = mul(float4(input.pos, 1.f), g_matWorld).xyz;
    worldPos += g_data[input.id].worldPos;

    output.viewPos = mul(float4(worldPos, 1.f), g_matView);
    output.uv = input.uv;
    output.id = input.id;

    return output;
}
//이제 지오메트리를 사용할거임. 정점 추가삭제를 할거임
struct GS_OUT
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
    uint id : SV_InstanceID;
};

[maxvertexcount(6)]  //일단 삼각형이 아니라 점 하나를 넘겨주고 그거를 지오메트리 단계에서 뱉어주는거임.
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)  //그려지는거면 정보 뱉어주고 아니면 넘겨
{
    GS_OUT output[4] =
    {
        (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f
    };

    VS_OUT vtx = input[0];
    uint id = (uint)vtx.id;
    if (0 == g_data[id].alive)  //살아있는거 아니면 리턴해서 점 하나도 안그려짐
        return;

    //그려지는거면 이제 물체 그려줘
    float ratio = g_data[id].curTime / g_data[id].lifeTime;
    float scale = ((g_float_1 - g_float_0) * ratio + g_float_0) / 2.f;

    // View Space
    output[0].position = vtx.viewPos + float4(-scale, scale, 0.f, 0.f);
    output[1].position = vtx.viewPos + float4(scale, scale, 0.f, 0.f);
    output[2].position = vtx.viewPos + float4(scale, -scale, 0.f, 0.f);
    output[3].position = vtx.viewPos + float4(-scale, -scale, 0.f, 0.f);

    // Projection Space  정점계산을 다시 하고
    output[0].position = mul(output[0].position, g_matProjection);
    output[1].position = mul(output[1].position, g_matProjection);
    output[2].position = mul(output[2].position, g_matProjection);
    output[3].position = mul(output[3].position, g_matProjection);
    //uv세팅
    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);
    //id세팅
    output[0].id = id;
    output[1].id = id;
    output[2].id = id;
    output[3].id = id;
    //삼각형 2개를 넣어주는중 그리고 위에서 뱉어.
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

RWStructuredBuffer<Particle> g_particle : register(u0); //1차원 배열
RWStructuredBuffer<ComputeShared> g_shared : register(u1);  //공용으로 활용할 버퍼

// CS_Main
// g_vec2_1 : DeltaTime / AccTime  시간
// g_int_0  : Particle Max Count
// g_int_1  : AddCount
// g_vec4_0 : MinLifeTime / MaxLifeTime / MinSpeed / MaxSpeed  랜덤하게 보이려고
[numthreads(1024, 1, 1)] //스레드 그룹으로 만들면 공통된 메모리를 사용할수도 있고 동기화도 가능
void CS_Main(int3 threadIndex : SV_DispatchThreadID)  //얘는 주의 해야 할게 순서가 보장되지를 않음. 그래서 뭔가 인덱스도 남의거를 건드린다거나 겹쳐서 한다면 뭔가... 겹치는 경우가 생기겠지..?
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

    g_shared[0].addCount = addCount;  //공용버퍼를 쓰고있는 부분임 그래서 병렬처리가 일어날수있어서 위험
    GroupMemoryBarrierWithGroupSync();  //그래서 베리어 쳐줘서 나머지 못들어오게 해주는거임. (동기화하는겨)

    if (g_particle[threadIndex.x].alive == 0)
    {
        while (true)
        {
            int remaining = g_shared[0].addCount;
            if (remaining <= 0)  //여기서는 이제 부활권 못가지면 또 나가면 됌. 
                break;

            int expected = remaining;
            int desired = remaining - 1;
            int originalValue;
            InterlockedCompareExchange(g_shared[0].addCount, expected, desired, originalValue); //addcount==expected가 같으면 desired값을 addcount로, 그리고 addcount는 마지막 인자값으로 
            //보통 이제 이런거 할때 InterlockedCompareExchange 이런 코드를 많이 써줌
            if (originalValue == expected)  //이게 같다는거는 내가 선착순으로 들어왔다는거임.
            {
                g_particle[threadIndex.x].alive = 1;  //승리했으니 나 부활
                break;
            }
        }

        //여기로 왔다는거는 이제 내가 부활한다는뜻임
        //이제 랜덤하게 세팅을 해줘야 하는데 아쉽게도 hlsl에는 랜덤함수가 업숴...
        if (g_particle[threadIndex.x].alive == 1)
        {
            //일단 걍 랜덤하게 해주려는거임...
            //노이즈 텍스쳐를 가져와서 거기의 uv를 매핑해서 추출해주는 식으로 랜덤을 생성하는 방식도 있음.
            //근데 일단 그냥.. 랜덤이면 되는거니까.. 일케해써 일단
            float x = ((float)threadIndex.x / (float)maxCount) + accTime; //경과된 시간을 계속 더해주는거를 랜덤 시드값으로 사용하는중임

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
            //초기화
            //이제부터는 내 데이터만 건드는거니까 동기화 기법은 필요가 없숴
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