#pragma once

enum PLANE_TYPE : uint8
{
	//평면 6개 만들거임 (카메라의 절두체 평면들)
	PLANE_FRONT,
	PLANE_BACK,
	PLANE_UP,
	PLANE_DOWN,
	PLANE_LEFT,
	PLANE_RIGHT,

	PLANE_END
};

class Frustum
{
public:
	void FinalUpdate();
	bool ContainsSphere(const Vec3& pos, float radius);  //안에 들어가있는지 체크

private:
	array<Vec4, PLANE_END> _planes;  //평면관리
};

