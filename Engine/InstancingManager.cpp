#include "pch.h"
#include "InstancingManager.h"
#include "InstancingBuffer.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"

void InstancingManager::Render(vector<shared_ptr<GameObject>>& gameObjects)
{
	map<uint64, vector<shared_ptr<GameObject>>> cache;  //인스턴스 아이디,같은 인스턴스 아이디를 사용(?)어쨌든 동일한 애들 오브젝트들

	for (shared_ptr<GameObject>& gameObject : gameObjects)
	{
		const uint64 instanceId = gameObject->GetMeshRenderer()->GetInstanceID();
		cache[instanceId].push_back(gameObject);
	}

	for (auto& pair : cache)
	{
		const vector<shared_ptr<GameObject>>& vec = pair.second;

		if (vec.size() == 1)  //그려지는 물체가 하나면 그냥 일반적으로 그림
		{
			vec[0]->GetMeshRenderer()->Render();
		}
		else  //인스턴싱 사용
		{
			const uint64 instanceId = pair.first;

			for (const shared_ptr<GameObject>& gameObject : vec)
			{
				InstancingParams params;
				params.matWorld = gameObject->GetTransform()->GetLocalToWorldMatrix();
				params.matWV = params.matWorld * Camera::S_MatView;
				params.matWVP = params.matWorld * Camera::S_MatView * Camera::S_MatProjection;

				AddParam(instanceId, params);  //해당 아이디에 해당 정보를 추가
			}

			shared_ptr<InstancingBuffer>& buffer = _buffers[instanceId];
			vec[0]->GetMeshRenderer()->Render(buffer);  //한방에 그려주기 위해서 인스턴싱 버퍼를 넘겨받아서 그려주는 역할	
		}
	}
}

void InstancingManager::ClearBuffer()
{
	for (auto& pair : _buffers)
	{
		shared_ptr<InstancingBuffer>& buffer = pair.second;
		buffer->Clear();
	}
}

void InstancingManager::AddParam(uint64 instanceId, InstancingParams& data)
{
	if (_buffers.find(instanceId) == _buffers.end())  //내가 사용하는 아이디의 인스턴싱 버퍼가 있는지 보고 없으면 생성
		_buffers[instanceId] = make_shared<InstancingBuffer>();

	_buffers[instanceId]->AddData(data);  //그 후 데이터 밀어넣음
}