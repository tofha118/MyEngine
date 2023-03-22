#include "pch.h"
#include "Light.h"
#include "Transform.h"
#include "Engine.h"
#include "Resources.h"
#include "Camera.h"
#include "Transform.h"
#include "Texture.h"
#include "SceneManager.h"

Light::Light() : Component(COMPONENT_TYPE::LIGHT)
{
	_shadowCamera = make_shared<GameObject>();  //그림자를 찍기 위한 용도로 내부적으로 간직함. 딱히 뭐 어디 출력하고 그러는건 아님.
	_shadowCamera->AddComponent(make_shared<Transform>());
	_shadowCamera->AddComponent(make_shared<Camera>());
	uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
	_shadowCamera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음
}


Light::~Light()
{
}

void Light::Render()
{
	assert(_lightIndex >= 0);

	GetTransform()->PushData();

	if (static_cast<LIGHT_TYPE>(_lightInfo.lightType) == LIGHT_TYPE::DIRECTIONAL_LIGHT)
	{
		shared_ptr<Texture> shadowTex = GET_SINGLE(Resources)->Get<Texture>(L"ShadowTarget");
		_lightMaterial->SetTexture(2, shadowTex);

		Matrix matVP = _shadowCamera->GetCamera()->GetViewMatrix() * _shadowCamera->GetCamera()->GetProjectionMatrix();
		_lightMaterial->SetMatrix(0, matVP);
	}
	else
	{
		float scale = 2 * _lightInfo.range;
		GetTransform()->SetLocalScale(Vec3(scale, scale, scale));
	}

	_lightMaterial->SetInt(0, _lightIndex);
	_lightMaterial->PushGraphicsData();

	_volumeMesh->Render();
}

//그림자 영억을 받은 모든 물체들을 그려준다
void Light::RenderShadow()
{
	_shadowCamera->GetCamera()->SortShadowObject();
	_shadowCamera->GetCamera()->Render_Shadow();
}

void Light::SetLightDirection(Vec3 direction)
{
	direction.Normalize();  //방향을 기존처럼 넣어주는건 똑같은데 트랜스폼에 어떤 방향을 바라보게 하면 그쪽으로 고개 휙 돌리게 하는거 추가

	_lightInfo.direction = direction;

	GetTransform()->LookAt(direction);
}

void Light::FinalUpdate()
{
	_lightInfo.position = GetTransform()->GetWorldPosition();

	//빛 좌표가 수정되었을 때 쉐이더 카메라도 똑같은 좌표로 세팅해줌.
	_shadowCamera->GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition());
	_shadowCamera->GetTransform()->SetLocalRotation(GetTransform()->GetLocalRotation());
	_shadowCamera->GetTransform()->SetLocalScale(GetTransform()->GetLocalScale());

	_shadowCamera->FinalUpdate();
}

void Light::SetLightType(LIGHT_TYPE type)  //해당하는 라이트에 따라 구현
{
	_lightInfo.lightType = static_cast<int32>(type);

	switch (type)
	{
	case LIGHT_TYPE::DIRECTIONAL_LIGHT:
		_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Rectangle");
		_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"DirLight");

		_shadowCamera->GetCamera()->SetScale(1.f);
		_shadowCamera->GetCamera()->SetFar(10000.f);
		_shadowCamera->GetCamera()->SetWidth(4096);
		_shadowCamera->GetCamera()->SetHeight(4096);
		break;
	case LIGHT_TYPE::POINT_LIGHT:
		_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Sphere");
		_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"PointLight");
		break;
	case LIGHT_TYPE::SPOT_LIGHT:
		_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Sphere");
		_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"PointLight");
		break;
	}
}