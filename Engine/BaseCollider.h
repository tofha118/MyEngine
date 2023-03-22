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

	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) = 0;  //어떤 관선을 쏴서 충돌하는지를 체크하는거

private:
	ColliderType _colliderType = {};
};