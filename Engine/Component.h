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
	FIXED_COMPONENT_COUNT = static_cast<uint8>(COMPONENT_TYPE::END) - 1  //MONO_BEHAVIOUR�� �� �������� �����ϱ� ����.
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

	virtual void ComponentUpdate() { }  //�ٲ�� ������Ʈ 


public:
	COMPONENT_TYPE GetType() { return _type; }
	bool IsValid() { return _gameObject.expired() == false; }

	shared_ptr<GameObject> GetGameObject();
	shared_ptr<Transform> GetTransform();
	shared_ptr<MeshRenderer> GetMeshRenderer();
	shared_ptr<Animator> GetAnimator();

private:
	friend class GameObject;  //���ӿ�����Ʈ������ ������ �Ǳ� ���ϱ� ������ ���ٱ��� ������.
	void SetGameObject(shared_ptr<GameObject> gameObject) { _gameObject = gameObject; }


protected:
	COMPONENT_TYPE _type;
	weak_ptr<GameObject> _gameObject;  //�� weak_ptr�ĸ� Component������ ���ӿ�����Ʈ�� �˾ƾ� �ϰ� ���ӿ�����Ʈ�� Component�� �����Ѿ� �ϱ⶧��


};

