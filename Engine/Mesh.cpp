#include "pch.h"
#include "Mesh.h"
#include "Engine.h"
#include "Material.h"
#include "InstancingBuffer.h"
#include "FBXLoader.h"
#include "StructuredBuffer.h"

Mesh::Mesh() : Object(OBJECT_TYPE::MESH)
{

}

Mesh::~Mesh()
{

}

void Mesh::Create(const vector<Vertex>& vertexBuffer, const vector<uint32>& indexBuffer)
{
	CreateVertexBuffer(vertexBuffer);
	CreateIndexBuffer(indexBuffer);
}

//void Mesh::Init(vector<Vertex>& vec)
//{
//	_vertexCount = static_cast<uint32>(vec.size());
//	uint32 bufferSize = _vertexCount * sizeof(Vertex);
//
//	//업로드 용도 ->원래 용도는 실시간으로 데이터를 전송할때 효율적. 나무같이 변하지 않는거는 노말타입이 더 효율적
//	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);  //어떤용도로 사용할것인가. 원래는 CPU에서 GPU로 데이터 복사하는 용도로만 사용하는건데 약간의 차이가 있긴한데 지금은 공용으로 활용하도록 하는거임.
//	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
//
//	
//	//디바이스를 통해서 하는거는 거의 즉시 실행되는거임.
//	//커맨드 리스트는 조금 나중에 (큐가 꽉 차면 전송할때 같이 전송)
//	//해당 공간 할당
//	DEVICE->CreateCommittedResource(
//		&heapProperty,
//		D3D12_HEAP_FLAG_NONE,
//		&desc,
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&_vertexBuffer));
//
//	// Copy the triangle data to the vertex buffer.
//	void* vertexDataBuffer = nullptr;
//	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
//	//vertexDataBuffer 이공간에 복사를 할수있게끔 연결해준거임.
//	_vertexBuffer->Map(0, &readRange, &vertexDataBuffer);
//	//그러고 복사한거임.
//	::memcpy(vertexDataBuffer, &vec[0], bufferSize);
//	//다 복사했다고 닫음
//	_vertexBuffer->Unmap(0, nullptr);
//
//	// Initialize the vertex buffer view.
//	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
//	_vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개 크기
//	_vertexBufferView.SizeInBytes = bufferSize; // 버퍼의 크기	
//}


void Mesh::Render(uint32 instanceCount, uint32 idx)
{
	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15)
	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_vecIndexInfo[idx].bufferView);

	GEngine->GetGraphicsDescHeap()->CommitTable();

	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_vecIndexInfo[idx].count, instanceCount, 0, 0, 0);
}

void Mesh::Render(shared_ptr<InstancingBuffer>& buffer, uint32 idx)
{
	D3D12_VERTEX_BUFFER_VIEW bufferViews[] = { _vertexBufferView, buffer->GetBufferView() };
	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 2, bufferViews);
	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_vecIndexInfo[idx].bufferView);

	GEngine->GetGraphicsDescHeap()->CommitTable();

	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_vecIndexInfo[idx].count, buffer->GetCount(), 0, 0, 0);
}

shared_ptr<Mesh> Mesh::CreateFromFBX(const FbxMeshInfo* meshInfo,  FBXLoader& loader)
{
	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->CreateVertexBuffer(meshInfo->vertices);

	for (const vector<uint32>& buffer : meshInfo->indices)
	{
		if (buffer.empty())
		{
			// FBX 파일이 이상하다. IndexBuffer가 없으면 에러 나니까 임시 처리
			vector<uint32> defaultBuffer{ 0 };
			mesh->CreateIndexBuffer(defaultBuffer);
		}
		else
		{
			mesh->CreateIndexBuffer(buffer);
		}
	}

	if (meshInfo->hasAnimation)
		mesh->CreateBonesAndAnimations(loader);


	return mesh;
}

#pragma region 원래 렌더 코드
//void Mesh::Render(uint32 instanceCount )
//{
//	//삼각형이라는 보장이 없으니까 일단 지워
//	//GRAPHICS_CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15)
//	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_indexBufferView);
//
//
//
//		// TODO
//	// 1) Buffer에다가 데이터 세팅
//	// 2) Buffer의 주소를 register에다가 전송
//	//GEngine->GetCB()->PushData(0, &_transform, sizeof(_transform));
//	//GEngine->GetCB()->PushData(1, &_transform, sizeof(_transform));
//
//		// 1) Buffer에다가 데이터 세팅
//	// 2) TableDescHeap에다가 CBV 전달
//	// 3) 모두 세팅이 끝났으면 TableDescHeap 커밋
//
//	//	GEngine->GetTableDescHeap()->SetCBV(handle, CBV_REGISTER::b0);
//
//		//GEngine->GetTableDescHeap()->SetSRV(_tex->GetCpuHandle(), SRV_REGISTER::t0);
//
//	//{
//	//	D3D12_CPU_DESCRIPTOR_HANDLE handle = GEngine->GetCB()->PushData(0, &_transform, sizeof(_transform));
//	//	GEngine->GetTableDescHeap()->SetCBV(handle, CBV_REGISTER::b1);
//	//}
//
//	GEngine->GetGraphicsDescHeap()->CommitTable();
//
//
//	//CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0);  //얘는 아예 버텍스를 읽어줌
//	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_indexCount, instanceCount, 0, 0, 0);  //이거는 이제 인덱스버퍼로 그려줌
//
//}
//
//void Mesh::Render(shared_ptr<InstancingBuffer>& buffer)
//{
//	//버퍼 넘겨줄때 두개 넘겨주는거 볼수있음
//	D3D12_VERTEX_BUFFER_VIEW bufferViews[] = { _vertexBufferView, buffer->GetBufferView() };
//	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 2, bufferViews);  //여기서 저 위에있는 버퍼 두개 넘겨줌
//	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_indexBufferView);
//
//	GEngine->GetGraphicsDescHeap()->CommitTable();
//
//	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_indexCount, buffer->GetCount(), 0, 0, 0);
//}
#pragma endregion

void Mesh::CreateVertexBuffer(const vector<Vertex>& buffer)
{
	_vertexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = _vertexCount * sizeof(Vertex);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer));

	// Copy the triangle data to the vertex buffer.
	void* vertexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	_vertexBuffer->Map(0, &readRange, &vertexDataBuffer);
	::memcpy(vertexDataBuffer, &buffer[0], bufferSize);
	_vertexBuffer->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개 크기
	_vertexBufferView.SizeInBytes = bufferSize; // 버퍼의 크기	
}

void Mesh::CreateIndexBuffer(const vector<uint32>& buffer)
{
	//임시적으로 만들어서 들고있다가 넣어줌
	uint32 indexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = indexCount * sizeof(uint32);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	ComPtr<ID3D12Resource> indexBuffer;
	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuffer));

	void* indexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	indexBuffer->Map(0, &readRange, &indexDataBuffer);
	::memcpy(indexDataBuffer, &buffer[0], bufferSize);
	indexBuffer->Unmap(0, nullptr);

	D3D12_INDEX_BUFFER_VIEW	indexBufferView;
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	indexBufferView.SizeInBytes = bufferSize;

	IndexBufferInfo info =
	{
		indexBuffer,
		indexBufferView,
		DXGI_FORMAT_R32_UINT,
		indexCount
	};

	_vecIndexInfo.push_back(info);

#pragma region 원래 createbuffer
	//_indexCount = static_cast<uint32>(buffer.size());
	//uint32 bufferSize = _indexCount * sizeof(uint32);

	//D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	//D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	//DEVICE->CreateCommittedResource(
	//	&heapProperty,
	//	D3D12_HEAP_FLAG_NONE,
	//	&desc,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&_indexBuffer));

	//void* indexDataBuffer = nullptr;
	//CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	//_indexBuffer->Map(0, &readRange, &indexDataBuffer);
	//::memcpy(indexDataBuffer, &buffer[0], bufferSize);
	//_indexBuffer->Unmap(0, nullptr);

	//_indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
	//_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	//_indexBufferView.SizeInBytes = bufferSize;
#pragma endregion

}



void Mesh::CreateBonesAndAnimations(class FBXLoader& loader)
{
#pragma region AnimClip  
	uint32 frameCount = 0;  //파싱한거 다시 분해해서 정보 넣어주는 작업
	vector<shared_ptr<FbxAnimClipInfo>>& animClips = loader.GetAnimClip();
	for (shared_ptr<FbxAnimClipInfo>& ac : animClips)
	{
		AnimClipInfo info = {};

		info.animName = ac->name;
		info.duration = ac->endTime.GetSecondDouble() - ac->startTime.GetSecondDouble();

		int32 startFrame = static_cast<int32>(ac->startTime.GetFrameCount(ac->mode));
		int32 endFrame = static_cast<int32>(ac->endTime.GetFrameCount(ac->mode));
		info.frameCount = endFrame - startFrame;

		info.keyFrames.resize(ac->keyFrames.size());

		const int32 boneCount = static_cast<int32>(ac->keyFrames.size());
		for (int32 b = 0; b < boneCount; b++)
		{
			auto& vec = ac->keyFrames[b];

			const int32 size = static_cast<int32>(vec.size());
			frameCount = max(frameCount, static_cast<uint32>(size));
			info.keyFrames[b].resize(size);

			for (int32 f = 0; f < size; f++)
			{
				FbxKeyFrameInfo& kf = vec[f];
				// FBX에서 파싱한 정보들로 채워준다
				KeyFrameInfo& kfInfo = info.keyFrames[b][f];
				kfInfo.time = kf.time;
				kfInfo.frame = static_cast<int32>(size);
				kfInfo.scale.x = static_cast<float>(kf.matTransform.GetS().mData[0]);
				kfInfo.scale.y = static_cast<float>(kf.matTransform.GetS().mData[1]);
				kfInfo.scale.z = static_cast<float>(kf.matTransform.GetS().mData[2]);
				kfInfo.rotation.x = static_cast<float>(kf.matTransform.GetQ().mData[0]);
				kfInfo.rotation.y = static_cast<float>(kf.matTransform.GetQ().mData[1]);
				kfInfo.rotation.z = static_cast<float>(kf.matTransform.GetQ().mData[2]);
				kfInfo.rotation.w = static_cast<float>(kf.matTransform.GetQ().mData[3]);
				kfInfo.translate.x = static_cast<float>(kf.matTransform.GetT().mData[0]);
				kfInfo.translate.y = static_cast<float>(kf.matTransform.GetT().mData[1]);
				kfInfo.translate.z = static_cast<float>(kf.matTransform.GetT().mData[2]);
			}
		}

		_animClips.push_back(info);
	}
#pragma endregion

#pragma region Bones
	vector<shared_ptr<FbxBoneInfo>>& bones = loader.GetBones();
	for (shared_ptr<FbxBoneInfo>& bone : bones)
	{
		BoneInfo boneInfo = {};
		boneInfo.parentIdx = bone->parentIndex;
		boneInfo.matOffset = GetMatrix(bone->matOffset);
		boneInfo.boneName = bone->boneName;
		_bones.push_back(boneInfo);
	}
#pragma endregion

#pragma region SkinData
	if (IsAnimMesh())
	{
		// BoneOffet 행렬
		const int32 boneCount = static_cast<int32>(_bones.size());
		vector<Matrix> offsetVec(boneCount);
		for (size_t b = 0; b < boneCount; b++)
			offsetVec[b] = _bones[b].matOffset;

		// OffsetMatrix StructuredBuffer 세팅
		_offsetBuffer = make_shared<StructuredBuffer>();
		_offsetBuffer->Init(sizeof(Matrix), static_cast<uint32>(offsetVec.size()), offsetVec.data());

		const int32 animCount = static_cast<int32>(_animClips.size());
		for (int32 i = 0; i < animCount; i++)
		{
			AnimClipInfo& animClip = _animClips[i];

			// 애니메이션 프레임 정보
			vector<AnimFrameParams> frameParams;
			frameParams.resize(_bones.size() * animClip.frameCount);

			for (int32 b = 0; b < boneCount; b++)
			{
				const int32 keyFrameCount = static_cast<int32>(animClip.keyFrames[b].size());
				for (int32 f = 0; f < keyFrameCount; f++)
				{
					int32 idx = static_cast<int32>(boneCount * f + b);

					frameParams[idx] = AnimFrameParams
					{
						Vec4(animClip.keyFrames[b][f].scale),
						animClip.keyFrames[b][f].rotation, // Quaternion
						Vec4(animClip.keyFrames[b][f].translate)
					};
				}
			}

			// StructuredBuffer 세팅
			_frameBuffer.push_back(make_shared<StructuredBuffer>());
			_frameBuffer.back()->Init(sizeof(AnimFrameParams), static_cast<uint32>(frameParams.size()), frameParams.data());
		}
	}
#pragma endregion
}

Matrix Mesh::GetMatrix(FbxAMatrix& matrix)
{
	Matrix mat;

	for (int32 y = 0; y < 4; ++y)
		for (int32 x = 0; x < 4; ++x)
			mat.m[y][x] = static_cast<float>(matrix.Get(y, x));

	return mat;
}