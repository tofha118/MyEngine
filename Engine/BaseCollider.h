#pragma once
#include "Component.h"

enum class ColliderType
{
	Sphere,
};

class BaseCollider : public Component
{
public:
	BaseCollider(ColliderType colliderType);
	virtual ~BaseCollider();

	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) = 0;  //� ������ ���� �浹�ϴ����� üũ�ϴ°�

private:
	ColliderType _colliderType = {};
};