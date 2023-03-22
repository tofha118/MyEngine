#pragma once
#include "Component.h"

enum class LIGHT_TYPE : uint8
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
};

struct LightColor
{
	Vec4	diffuse;
	Vec4	ambient;
	Vec4	specular;
};

struct LightInfo  //조명의 정보
{
	LightColor	color;		//색
	Vec4		position;	//위치
	Vec4		direction;	//빛이 쏘는 방향 POINT_LIGHT의 경우 중요하지는 않음. 나머지경우는 매우 중요
	int32		lightType;	//빛 타입
	float		range;		
	float		angle;		
	int32		padding;
};

//다른 애들과 다르게 한번에 넘겨줌
struct LightParams //쉐이더로 넘겨줄 정보
{
	uint32		lightCount;
	Vec3		padding;
	LightInfo	lights[50];
};

class Light : public Component
{
public:
	Light();
	virtual ~Light();

	virtual void FinalUpdate() override;
	void Render();
	void RenderShadow();

public:

	LIGHT_TYPE GetLightType() { return static_cast<LIGHT_TYPE>(_lightInfo.lightType); }

	const LightInfo& GetLightInfo() { return _lightInfo; }

	void SetLightDirection(Vec3 direction);

	//void SetLightDirection(const Vec3& direction) { _lightInfo.direction = direction; } 기존엔 그냥 방향만 바꿨는데 쉐이더 추가되면서 카메라의 트랜스를 이동시켜야댐

	void SetDiffuse(const Vec3& diffuse) { _lightInfo.color.diffuse = diffuse; }
	void SetAmbient(const Vec3& ambient) { _lightInfo.color.ambient = ambient; }
	void SetSpecular(const Vec3& specular) { _lightInfo.color.specular = specular; }

	void SetLightType(LIGHT_TYPE type);
	void SetLightRange(float range) { _lightInfo.range = range; }
	void SetLightAngle(float angle) { _lightInfo.angle = angle; }

	void SetLightIndex(int8 index) { _lightIndex = index; }

private:
	LightInfo _lightInfo = {};  

	int8 _lightIndex = -1;
	shared_ptr<class Mesh> _volumeMesh;  //내가 어떤 영역을 차지하는지
	shared_ptr<class Material> _lightMaterial;

	shared_ptr<GameObject> _shadowCamera;  //그림자 계산을 위해 라이트와 같은 영역을 찍어야 하기 때문에
	 
};


