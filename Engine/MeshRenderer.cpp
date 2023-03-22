#include "pch.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "InstancingBuffer.h"
#include "Resources.h"
#include "Animator.h"

MeshRenderer::MeshRenderer() : Component(COMPONENT_TYPE::MESH_RENDERER)
{

}

MeshRenderer::~MeshRenderer()
{

}

void MeshRenderer::SetMaterial(shared_ptr<Material> material, uint32 idx)
{
	if (_materials.size() <= static_cast<size_t>(idx))
		_materials.resize(static_cast<size_t>(idx + 1));

	_materials[idx] = material;
}

void MeshRenderer::Render()
{
	for (uint32 i = 0; i < _materials.size(); i++)
	{
		shared_ptr<Material>& material = _materials[i];

		if (material == nullptr || material->GetShader() == nullptr)
			continue;

		GetTransform()->PushData();

		//만약 애니메이션이 있는거라면 정보 세팅
		if (GetAnimator())
		{
			GetAnimator()->PushData();
			material->SetInt(1, 1);
		}

		material->PushGraphicsData();
		_mesh->Render(1, i);
	}
}


void MeshRenderer::RenderShadow()
{
	GetTransform()->PushData();
	GET_SINGLE(Resources)->Get<Material>(L"Shadow")->PushGraphicsData();
	_mesh->Render();
}

//void MeshRenderer::Render(shared_ptr<InstancingBuffer>& buffer)
//{
//	buffer->PushData();
//	_material->PushGraphicsData();
//	_mesh->Render(buffer);
//}

void MeshRenderer::Render(shared_ptr<InstancingBuffer>& buffer)
{
	for (uint32 i = 0; i < _materials.size(); i++)
	{
		shared_ptr<Material>& material = _materials[i];

		if (material == nullptr || material->GetShader() == nullptr)
			continue;

		//만약 애니메이션이 있는거라면 정보 세팅
		if (GetAnimator())
		{
			GetAnimator()->PushData();
			material->SetInt(1, 1);
		}

		buffer->PushData();
		material->PushGraphicsData();
		_mesh->Render(buffer, i);
	}
}

//대충 아이디 생성해주는거임
//uint64 MeshRenderer::GetInstanceID()
//{
//	if (_mesh == nullptr || _material == nullptr)
//		return 0;
//
//	//uint64 id = (_mesh->GetID() << 32) | _material->GetID();
//	InstanceID instanceID{ _mesh->GetID(), _material->GetID() };
//	return instanceID.id;
//}


uint64 MeshRenderer::GetInstanceID()
{
	if (_mesh == nullptr || _materials.empty())
		return 0;

	//uint64 id = (_mesh->GetID() << 32) | _material->GetID();
	InstanceID instanceID{ _mesh->GetID(), _materials[0]->GetID() };
	return instanceID.id;
}