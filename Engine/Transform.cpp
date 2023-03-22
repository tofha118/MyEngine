#include "pch.h"
#include "Transform.h"
#include "Engine.h"
#include "Camera.h"


//본인의 부모 (Transform의 부모는 Component) 라고 알려줌.
Transform::Transform() : Component(COMPONENT_TYPE::TRANSFORM)
{

}

Transform::~Transform()
{

}


void Transform::LookAt(const Vec3& dir)  //내가 바라보고싶은 방향을 z축이 되도록.
{
	Vec3 front = dir;
	front.Normalize();  //내가 바라보고싶은 방향을 front하고있음.

	Vec3 right = Vec3::Up.Cross(dir);
	if (right == Vec3::Zero)
		right = Vec3::Forward.Cross(dir);

	right.Normalize();

	Vec3 up = front.Cross(right);
	up.Normalize();

	Matrix matrix = XMMatrixIdentity();  //회전 행렬 구했음
	matrix.Right(right);
	matrix.Up(up);
	matrix.Backward(front);

	_localRotation = DecomposeRotationMatrix(matrix);  //분해하는거임.
}

bool Transform::CloseEnough(const float& a, const float& b, const float& epsilon)
{
	return (epsilon > std::abs(a - b));
}


Vec3 Transform::DecomposeRotationMatrix(const Matrix& rotation)
{
	Vec4 v[4];
	XMStoreFloat4(&v[0], rotation.Right());
	XMStoreFloat4(&v[1], rotation.Up());
	XMStoreFloat4(&v[2], rotation.Backward());
	XMStoreFloat4(&v[3], rotation.Translation());

	Vec3 ret;
	if (CloseEnough(v[0].z, -1.0f))
	{
		float x = 0;
		float y = XM_PI / 2;
		float z = x + atan2(v[1].x, v[2].x);
		ret = Vec3{ x, y, z };
	}
	else if (CloseEnough(v[0].z, 1.0f))
	{
		float x = 0;
		float y = -XM_PI / 2;
		float z = -x + atan2(-v[1].x, -v[2].x);
		ret = Vec3{ x, y, z };
	}
	else
	{
		float y1 = -asin(v[0].z);
		float y2 = XM_PI - y1;

		float x1 = atan2f(v[1].z / cos(y1), v[2].z / cos(y1));
		float x2 = atan2f(v[1].z / cos(y2), v[2].z / cos(y2));

		float z1 = atan2f(v[0].y / cos(y1), v[0].x / cos(y1));
		float z2 = atan2f(v[0].y / cos(y2), v[0].x / cos(y2));

		if ((std::abs(x1) + std::abs(y1) + std::abs(z1)) <= (std::abs(x2) + std::abs(y2) + std::abs(z2)))
		{
			ret = Vec3{ x1, y1, z1 };
		}
		else
		{
			ret = Vec3{ x2, y2, z2 };
		}
	}

	return ret;
}

void Transform::FinalUpdate()
{
	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z);
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);
	

	_matLocal = matScale * matRotation * matTranslation;
	_matWorld = _matLocal;

	shared_ptr<Transform> parent = GetParent().lock();
	if (parent != nullptr)  //부모 오브젝트가 있다면 부모의 월드 매트릭스를 긁어와서 계산해줘야댐
	{ 
		_matWorld *= parent->GetLocalToWorldMatrix();
	}
}

void Transform::PushData()
{
	//필요할 정보 모두 세팅
	TransformParams transformParams = {};
	transformParams.matWorld = _matWorld;
	transformParams.matView = Camera::S_MatView;
	transformParams.matProjection = Camera::S_MatProjection;
	transformParams.matWV = _matWorld * Camera::S_MatView;
	transformParams.matWVP = _matWorld * Camera::S_MatView * Camera::S_MatProjection;
	transformParams.matViewInv = Camera::S_MatView.Invert();  //이거 빼서 그림자가 카메라랑 같이 움직여써 ㅋㅋ
	//Matrix matWVP = _matWorld * Camera::S_MatView * Camera::S_MatProjection; //월드,카메라 뷰,WVP(카메라의 프로젝션)
	//CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushData(&matWVP, sizeof(matWVP));
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushGraphicsData(&transformParams, sizeof(transformParams));

}


void Transform::ComponentUpdate() /* Imgui Update */
{
	if (ImGui::CollapsingHeader("Transform")) {
		const float value = 1.0f;
		Vec3 tempVec = Vec3::Zero;

		tempVec = _localPosition;
		ImGui::DragFloat3("Position", (float*)&tempVec, value, -10000.0f, 10000.0f);
		_localPosition = tempVec;			// 프로퍼티로 하는 이유가 있음

		tempVec = _localRotation;
		ImGui::DragFloat3("Rotation", (float*)&tempVec, value / 36, -10000.0f, 10000.0f);
		_localRotation = tempVec;			// 프로퍼티로 하는 이유가 있음

		tempVec = _localScale;
		ImGui::DragFloat3("Scale", (float*)&tempVec, value, -10000.0f, 10000.0f);
		_localScale = tempVec;				// 프로퍼티로 하는 이유가 있음
	}
}

