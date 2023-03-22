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
			_cbvBuffer->Unmap(0, nullptr);  //�Ҹ��� �ɶ� ����� ���ִ´�����(���� �����ִ� ����)

		_cbvBuffer = nullptr;
	}
}



void ConstantBuffer::Init(CBV_REGISTER reg,uint32 size, uint32 count)
{
	_reg = reg;

	// ��� ���۴� 256 ����Ʈ ����� ������ �Ѵ�  (��Ģ��)
	// 0 256 512 768
	_elementSize = (size + 255) & ~255;  //Ȥ�ö� ���ȵ������ �������ִ´�����.
	_elementCount = count;

	CreateBuffer();
	CreateView();
}

void ConstantBuffer::CreateBuffer()
{
	uint32 bufferSize = _elementSize * _elementCount;
	//���� �Ҵ� �޴���
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_cbvBuffer));

	//_mappedBuffer�� ���ؼ� ���� cpu���� _mappedBuffer�� ���� �����͸� �о������ �ǰ���
	_cbvBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_mappedBuffer));
	//�ؿ� �ּ��� GPU�� ���� �а��ִµ� ���� ������ �ϸ� ū�ϳ�����?
	//�׷��� �츮�� GPU�� �а������� �ǵ�� �ȵ�..(�Ѹ���� ����ȭ�� ����� �ϴµ�)
	//�츮�� ���� ���ص� �Ǵ°� fence waitSync�� ��ٸ��°� ���ִϱ� ���������(�ٵ� �̰� �� ���Ĥ�)
	// 
	// We do not need to unmap until we are done with the resource.  However, we must not write to
	// the resource while it is in use by the GPU (so we must use synchronization techniques).
}


void ConstantBuffer::CreateView()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvDesc = {};
	cbvDesc.NumDescriptors = _elementCount;  //���۸� ���� �� �̰��� ��ŭ ������� ������ �� ���鶧�� �� ���� ��� ����
	cbvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  //�̷��� �⺻���� �־���� ȿ�������� ������.
	cbvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DEVICE->CreateDescriptorHeap(&cbvDesc, IID_PPV_ARGS(&_cbvHeap));

	_cpuHandleBegin = _cbvHeap->GetCPUDescriptorHandleForHeapStart();
	_handleIncrementSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); //GPU���� ����� �ٸ��⶧���� �̷��� �ؿ;� �Ѵٰ� ��.

	for (uint32 i = 0; i < _elementCount; ++i)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle = GetCpuHandle(i);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = _cbvBuffer->GetGPUVirtualAddress() + static_cast<uint64>(_elementSize) * i;  //������ �ּҿ�
		cbvDesc.SizeInBytes = _elementSize;   // CB size is required to be 256-byte aligned. ������ ũ�⸦ �����ϴ°���.

		DEVICE->CreateConstantBufferView(&cbvDesc, cbvHandle);  //�� �ϼ�
	}
}

void ConstantBuffer::Clear()
{
	_currentIndex = 0;
}

//�ۿ��� �̰Ÿ� ���ָ� ���� ����� �������Ϳ� �ø��°� (���)���� �Ǵ°���.
void ConstantBuffer::PushGraphicsData( void* buffer, uint32 size)
{
	assert(_currentIndex < _elementCount);  //�츮�� �ʹ� ���� ����ϸ�(���� ũ�⸦ �����) (����� �뵵��)
	assert(_elementSize == ((size + 255) & ~255));  //�̰͵� ����� ������ �������


	::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size);  //���۸� �������ִ� �κ���. ��ǻ� ����Ͼ

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = GetCpuHandle(_currentIndex);
	GEngine->GetGraphicsDescHeap()->SetCBV(cpuHandle, _reg);

	//�̰� �����ؾߴ� �� �κп��� ũ���ð� ���ٰ� �Ѵ�. 
	//D3D12_GPU_VIRTUAL_ADDRESS address = GetGpuVirtualAddress(_currentIndex);
	//CMD_LIST->SetGraphicsRootConstantBufferView(rootParamIndex, address);  //���߿� �����̵ǰ����� (Ŀ�ǵ� ť�ϱ�) ���ʿ� (�ش��ּҸ� �����ؼ� �۾��϶�� �������Ϳ� �ø��°���)
	//�Ʊ� ��Ʈ �ñ״��Ŀ�(init) ���� �����ѰŸ� ��� ���� rootParamIndex�̰ŷ� ����
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

//�ϳ��� ������
void ConstantBuffer::SetGraphicsGlobalData(void* buffer, uint32 size)
{
	assert(_elementSize == ((size + 255) & ~255));
	::memcpy(&_mappedBuffer[0], buffer, size);
	GRAPHICS_CMD_LIST->SetGraphicsRootConstantBufferView(0, GetGpuVirtualAddress(0));
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGpuVirtualAddress(uint32 index)
{
	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = _cbvBuffer->GetGPUVirtualAddress();  //������ ���� �ּҸ� �޾ƿ���
	objCBAddress += index * _elementSize;  //������ ���° ĭ���� ����ؼ� �ּҸ� �Ѱ��ִ°���. (���۰� �������̾ƴ϶� �迭���� ����Ǿ��ִ� �޸�)
	return objCBAddress;
}

D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetCpuHandle(uint32 index)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandleBegin, index * _handleIncrementSize);  
}