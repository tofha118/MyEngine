#pragma once
#include "BaseCollider.h"

class SphereCollider : public BaseCollider
{
public:
	SphereCollider();
	virtual ~SphereCollider();

	virtual void FinalUpdate() override;
	void ComponentUpdate() override;
	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;

	void SetRadius(float radius) { _radius = radius; }
	void SetCenter(Vec3 center) { _center = center; }

	void ShowCollider();

	bool& GetActive() { return _isShowCollider; }


private:
	// Local 기준
	float		_radius = 1.f;  //반지름
	Vec3		_center = Vec3(0, 0, 0);  

	BoundingSphere _boundingSphere;  //구의 볼륨

	bool _isShowCollider = false;

};

