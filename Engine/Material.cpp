#include "pch.h"
#include "Material.h"
#include "Engine.h"

Material::Material() : Object(OBJECT_TYPE::MATERIAL)
{

}

Material::~Material()
{

}


void Material::PushGraphicsData()
{
	// CBV 업로드
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushGraphicsData(&_params, sizeof(_params));

	// SRV 업로드
	for (size_t i = 0; i < _textures.size(); i++)
	{
		if (_textures[i] == nullptr)
			continue;

		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t0) + i);
		GEngine->GetGraphicsDescHeap()->SetSRV(_textures[i]->GetSRVHandle(), reg);
	}

	// 파이프라인 세팅
	_shader->Update();
}

void Material::PushComputeData()
{
	// CBV 업로드
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushComputeData(&_params, sizeof(_params));

	// SRV 업로드
	for (size_t i = 0; i < _textures.size(); i++)
	{
		if (_textures[i] == nullptr)
			continue;

		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t0) + i);
		GEngine->GetComputeDescHeap()->SetSRV(_textures[i]->GetSRVHandle(), reg);  //여기가 바뀜
	}

	// 파이프라인 세팅
	_shader->Update();
}

void Material::Dispatch(uint32 x, uint32 y, uint32 z)
{
	// CBV + SRV + SetPipelineState
	PushComputeData();  //파이프라인을 먼저 세팅해주고

	// SetDescriptorHeaps + SetComputeRootDescriptorTable
	GEngine->GetComputeDescHeap()->CommitTable();  //커밋 테이블 때려서 커밋하는 단계

	COMPUTE_CMD_LIST->Dispatch(x, y, z);  //실제 실행하는 부분

	GEngine->GetComputeCmdQueue()->FlushComputeCommandQueue();
}

shared_ptr<Material> Material::Clone()
{
	shared_ptr<Material> material = make_shared<Material>();

	material->SetShader(_shader);
	material->_params = _params;
	material->_textures = _textures;

	return material;
}