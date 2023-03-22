#include "pch.h"
#include "ConstantBuffer.h"
#include "Engine.h"


ConstantBuffer::ConstantBuffer()
{
}

ConstantBuffer::~ConstantBuffer()
{
	if (_cbvBuffer)
	{
		if (_cbvBuffer != nullptr)
			_cbvBuffer->Unmap(0, nullptr);  //소멸이 될때 언맵을 해주는느낌임(연결 끊어주는 느낌)

		_cbvBuffer = nullptr;
	}
}



void ConstantBuffer::Init(CBV_REGISTER reg,uint32 size, uint32 count)
{
	_reg = reg;

	// 상수 버퍼는 256 바이트 배수로 만들어야 한다  (원칙임)
	// 0 256 512 768
	_elementSize = (size + 255) & ~255;  //혹시라도 말안들을까봐 보정해주는느낌임.
	_elementCount = count;

	CreateBuffer();
	CreateView();
}

void ConstantBuffer::CreateBuffer()
{
	uint32 bufferSize = _elementSize * _elementCount;
	//버퍼 할당 받는중
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_cbvBuffer));

	//_mappedBuffer를 통해서 관리 cpu에서 _mappedBuffer를 통해 데이터를 밀어넣으면 되겠찡
	_cbvBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_mappedBuffer));
	//밑에 주석은 GPU가 만약 읽고있는데 내가 쓰려고 하면 큰일나겠지?
	//그래서 우리는 GPU가 읽고있을때 건들면 안돼..(한마디로 동기화를 맞춰야 하는데)
	//우리는 걱정 안해도 되는게 fence waitSync로 기다리는거 해주니까 상관없ㅋㅋ(근데 이거 좀 무식ㅋ)
	// 
	// We do not need to unmap until we are done with the resource.  However, we must not write to
	// the resource while it is in use by the GPU (so we must use synchronization techniques).
}


void ConstantBuffer::CreateView()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvDesc = {};
	cbvDesc.NumDescriptors = _elementCount;  //버퍼를 만들 때 이개수 만큼 만들었기 때문에 힙 만들때도 이 개수 대로 만듦
	cbvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  //이렇게 기본으로 넣어야지 효율적으로 동작함.
	cbvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DEVICE->CreateDescriptorHeap(&cbvDesc, IID_PPV_ARGS(&_cbvHeap));

	_cpuHandleBegin = _cbvHeap->GetCPUDescriptorHandleForHeapStart();
	_handleIncrementSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); //GPU마다 사양이 다르기때문에 이렇게 해와야 한다고 함.

	for (uint32 i = 0; i < _elementCount; ++i)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle = GetCpuHandle(i);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = _cbvBuffer->GetGPUVirtualAddress() + static_cast<uint64>(_elementSize) * i;  //버퍼의 주소와
		cbvDesc.SizeInBytes = _elementSize;   // CB size is required to be 256-byte aligned. 버퍼의 크기를 묘사하는거임.

		DEVICE->CreateConstantBufferView(&cbvDesc, cbvHandle);  //뷰 완성
	}
}

void ConstantBuffer::Clear()
{
	_currentIndex = 0;
}

//밖에서 이거를 해주면 버퍼 만들고 레지스터에 올리는거 (대기)까지 되는거임.
void ConstantBuffer::PushGraphicsData( void* buffer, uint32 size)
{
	assert(_currentIndex < _elementCount);  //우리가 너무 많이 사용하면(버퍼 크기를 벗어나면) (디버그 용도임)
	assert(_elementSize == ((size + 255) & ~255));  //이것도 디버그 용으로 사용했음


	::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size);  //버퍼를 복사해주는 부분임. 사실상 즉시일어남

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = GetCpuHandle(_currentIndex);
	GEngine->GetGraphicsDescHeap()->SetCBV(cpuHandle, _reg);

	//이거 삭제해야댐 이 부분에서 크러시가 난다고 한다. 
	//D3D12_GPU_VIRTUAL_ADDRESS address = GetGpuVirtualAddress(_currentIndex);
	//CMD_LIST->SetGraphicsRootConstantBufferView(rootParamIndex, address);  //나중에 실행이되겠지만 (커맨드 큐니까) 위쪽에 (해당주소를 참고해서 작업하라고 레지스터에 올리는거임)
	//아까 루트 시그니쳐에(init) 변수 선언한거를 어떤거 쓸지 rootParamIndex이거로 구분
	_currentIndex++;

}


void ConstantBuffer::PushComputeData(void* buffer, uint32 size)
{
	assert(_currentIndex < _elementCount);
	assert(_elementSize == ((size + 255) & ~255));

	::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size);

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = GetCpuHandle(_currentIndex);
	GEngine->GetComputeDescHeap()->SetCBV(cpuHandle, _reg);

	_currentIndex++;
}

//하나의 데이터
void ConstantBuffer::SetGraphicsGlobalData(void* buffer, uint32 size)
{
	assert(_elementSize == ((size + 255) & ~255));
	::memcpy(&_mappedBuffer[0], buffer, size);
	GRAPHICS_CMD_LIST->SetGraphicsRootConstantBufferView(0, GetGpuVirtualAddress(0));
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGpuVirtualAddress(uint32 index)
{
	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = _cbvBuffer->GetGPUVirtualAddress();  //버퍼의 시작 주소를 받아오고
	objCBAddress += index * _elementSize;  //본인이 몇번째 칸인지 계산해서 주소를 넘겨주는거임. (버퍼가 띄엄띄엄이아니라 배열마냥 연결되어있는 메모리)
	return objCBAddress;
}

D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetCpuHandle(uint32 index)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandleBegin, index * _handleIncrementSize);  
}