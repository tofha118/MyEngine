#pragma once

enum class CONSTANT_BUFFER_TYPE : uint8
{
	GLOBAL,  //���� ������ b0
	TRANSFORM,
	MATERIAL,
	END
};

enum
{
	CONSTANT_BUFFER_COUNT = static_cast<uint8>(CONSTANT_BUFFER_TYPE::END)
};
 

class ConstantBuffer
{

public:
	ConstantBuffer();
	~ConstantBuffer();

	void Init(CBV_REGISTER reg, uint32 size, uint32 count);
	

	void Clear();
	void PushGraphicsData( void* buffer, uint32 size);
	void SetGraphicsGlobalData(void* buffer, uint32 size);
	void PushComputeData(void* buffer, uint32 size);

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(uint32 index);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32 index);  //��Ʈ���̺�


private:
	void CreateBuffer();
	void CreateView();  //��Ʈ���̺�


private:
	ComPtr<ID3D12Resource>	_cbvBuffer;  //ComPtr�̰� ������ ��κ� GPU�� ������ ���� �ϳ��� ����ϴ°� �ƴ϶� ��ü�� ũ��..?
	BYTE* _mappedBuffer = nullptr;
	uint32					_elementSize = 0;  //���� ������
	uint32					_elementCount = 0;  //���� ����

	ComPtr<ID3D12DescriptorHeap>		_cbvHeap;  //��Ʈ���̺�
	D3D12_CPU_DESCRIPTOR_HANDLE			_cpuHandleBegin = {};  //�����ڵ� �ּ�
	uint32								_handleIncrementSize = 0;  //��ĭ�� ����� ���� �ڵ�� �Ѿ���ִ���.

	uint32					_currentIndex = 0;  //������ ����ߴ��� ���� (���� �� �׷������� �ٽ� 0���� �����ؼ� �����Ѵٰ� �����ϸ� ��)

	CBV_REGISTER			_reg = {};

};

