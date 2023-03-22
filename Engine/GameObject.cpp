#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "MonoBehaviour.h"
#include "Camera.h"
#include "Light.h"
#include "ParticleSystem.h"
#include "Terrain.h"
#include "BaseCollider.h"
#include "Animator.h"


GameObject::GameObject() : Object(OBJECT_TYPE::GAMEOBJECT)
{

}


GameObject::~GameObject()
{

}
//void GameObject::Init()
//{
//	//유니티 처럼 기본 컴포넌트 Transform 추가
//	AddComponent(make_shared<Transform>());
//}

//내가 들고있는 모든 컴포넌트의 함수를 모두 호출
void GameObject::Awake()
{
	for (shared_ptr<Component>& component : _components)
	{
		if (component)
			component->Awake();
	}

	for (shared_ptr<MonoBehaviour>& script : _scripts)
	{
		script->Awake();
	}
}

void GameObject::Start()
{
	for (shared_ptr<Component>& component : _components)
	{
		if (component)
			component->Start();
	}

	for (shared_ptr<MonoBehaviour>& script : _scripts)
	{
		script->Start();
	}
}

void GameObject::Update()
{
	for (shared_ptr<Component>& component : _components)
	{
		if (component)
			component->Update();
	}

	for (shared_ptr<MonoBehaviour>& script : _scripts)
	{
		script->Update();
	}
}

void GameObject::LateUpdate()
{
	for (shared_ptr<Component>& component : _components)
	{
		if (component)
			component->LateUpdate();
	}

	for (shared_ptr<MonoBehaviour>& script : _scripts)
	{
		script->LateUpdate();
	}
}

void GameObject::FinalUpdate()
{
	for (shared_ptr<Component>& component : _components)
	{
		if (component)
			component->FinalUpdate();
	}
}

shared_ptr<Component> GameObject::GetFixedComponent(COMPONENT_TYPE type)
{
	uint8 index = static_cast<uint8>(type);  //타입을 반환해서
	assert(index < FIXED_COMPONENT_COUNT);  //디버그 해서 체크 해준다음에 
	return _components[index];  //컴포넌트를 뱉어주는 역할임
}

//게임 인덱스 번호로 슬롯 추출(컴포넌트추출)해서 뱉어주는 부분임.
shared_ptr<Transform> GameObject::GetTransform()
{
	//uint8 index = static_cast<uint8>(COMPONENT_TYPE::TRANSFORM);
	//return static_pointer_cast<Transform>(_components[index]);
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::TRANSFORM);
	return static_pointer_cast<Transform>(component);  //위랑독같은거임
}

shared_ptr<MeshRenderer> GameObject::GetMeshRenderer()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::MESH_RENDERER);
	return static_pointer_cast<MeshRenderer>(component);
}

shared_ptr<Camera> GameObject::GetCamera()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::CAMERA);
	return static_pointer_cast<Camera>(component);
}

shared_ptr<Light> GameObject::GetLight()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::LIGHT);
	return static_pointer_cast<Light>(component);
}

shared_ptr<ParticleSystem> GameObject::GetParticleSystem()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::PARTICLE_SYSTEM);
	return static_pointer_cast<ParticleSystem>(component);
}

shared_ptr<Terrain> GameObject::GetTerrain()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::TERRAIN);
	return static_pointer_cast<Terrain>(component);
}

shared_ptr<BaseCollider> GameObject::GetCollider()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::COLLIDER);
	return static_pointer_cast<BaseCollider>(component);
}

shared_ptr<Animator> GameObject::GetAnimator()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::ANIMATOR);
	return static_pointer_cast<Animator>(component);
}

void GameObject::AddComponent(shared_ptr<Component> component)
{
	//컴포넌트에 본인의 포인터를 넘겨줌. 이부분때문에 enable_shared_from_this<GameObject>상속 받음
	component->SetGameObject(shared_from_this());

	uint8 index = static_cast<uint8>(component->GetType());
	if (index < FIXED_COMPONENT_COUNT)
	{
		_components[index] = component;
	}
	else
	{
		_scripts.push_back(dynamic_pointer_cast<MonoBehaviour>(component));
	}
}