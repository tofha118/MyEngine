#pragma once
#include "Object.h"
enum class COMPONENT_TYPE : uint8
{
	TRANSFORM,
	MESH_RENDERER,
	CAMERA,
	LIGHT,
	PARTICLE_SYSTEM,
	TERRAIN,
	COLLIDER,
	ANIMATOR,
	// ...
	MONO_BEHAVIOUR,
	END,
};

enum
{
	FIXED_COMPONENT_COUNT = static_cast<uint8>(COMPONENT_TYPE::END) - 1  //MONO_BEHAVIOUR를 뺀 나머지를 관리하기 위함.
};


class GameObject;
class Transform;
class MeshRenderer;
class Animator;
class Component : Object
{
public:
	Component(COMPONENT_TYPE type);
	virtual ~Component();

public:
	virtual void Awake() { }  //1
	virtual void Start() { }  //2
	virtual void Update() { }  //3
	virtual void LateUpdate() { }  //4
	virtual void FinalUpdate() { }  //5

	virtual void ComponentUpdate() { }  //바뀐거 업데이트 


public:
	COMPONENT_TYPE GetType() { return _type; }
	bool IsValid() { return _gameObject.expired() == false; }

	shared_ptr<GameObject> GetGameObject();
	shared_ptr<Transform> GetTransform();
	shared_ptr<MeshRenderer> GetMeshRenderer();
	shared_ptr<Animator> GetAnimator();

private:
	friend class GameObject;  //게임오브젝트에서만 실행이 되길 원하기 때문에 접근권한 열어줌.
	void SetGameObject(shared_ptr<GameObject> gameObject) { _gameObject = gameObject; }


protected:
	COMPONENT_TYPE _type;
	weak_ptr<GameObject> _gameObject;  //왜 weak_ptr냐면 Component에서도 게임오브젝트를 알아야 하고 게임오브젝트도 Component를 가리켜야 하기때문


};

