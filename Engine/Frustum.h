#pragma once

enum PLANE_TYPE : uint8
{
	//��� 6�� ������� (ī�޶��� ����ü ����)
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
	bool ContainsSphere(const Vec3& pos, float radius);  //�ȿ� ���ִ��� üũ

private:
	array<Vec4, PLANE_END> _planes;  //������
};

