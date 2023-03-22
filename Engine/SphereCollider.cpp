#include "pch.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include "Transform.h"

#include "imgui.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Shader.h"
#include "Resources.h"


SphereCollider::SphereCollider() : BaseCollider(ColliderType::Sphere)
{

}

SphereCollider::~SphereCollider()
{

}

void SphereCollider::ComponentUpdate()
{
	if (ImGui::CollapsingHeader("SphereCollider")) /* Imgui */  //이름같은거
	{
		ImGui::Checkbox("Show Collider", &GetActive());
		ImGui::DragFloat3("Radius", &_radius, 0.01f, 0.0f, 1000.0f);

		ShowCollider();
	}
}


void SphereCollider::FinalUpdate()
{
	_boundingSphere.Center = GetGameObject()->GetTransform()->GetWorldPosition();

	Vec3 scale = GetGameObject()->GetTransform()->GetLocalScale();
	_boundingSphere.Radius = _radius * max(max(scale.x, scale.y), scale.z);
}

bool SphereCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return _boundingSphere.Intersects(rayOrigin, rayDir, OUT distance);  //광선의 시작점, 방향
}

void SphereCollider::ShowCollider() /* WireFrame 모드 설정 */
{
	if (_isShowCollider)
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"WireFrame");  //쉐이더 선 나오는거로 표시
		GetMeshRenderer()->GetMaterial()->SetShader(shader);
	}
	else
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Deferred");
		GetMeshRenderer()->GetMaterial()->SetShader(shader);
	}
}