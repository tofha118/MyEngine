#pragma once

enum class CONSTANT_BUFFER_TYPE : uint8
{
	GLOBAL,  //전역 데이터 b0
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
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32 index);  //루트테이블


private:
	void CreateBuffer();
	void CreateView();  //루트테이블


private:
	ComPtr<ID3D12Resource>	_cbvBuffer;  //ComPtr이게 붙으면 대부분 GPU쪽 변수임 버퍼 하나를 얘기하는게 아니라 전체의 크기..?
	BYTE* _mappedBuffer = nullptr;
	uint32					_elementSize = 0;  //버퍼 사이즈
	uint32					_elementCount = 0;  //버퍼 갯수

	ComPtr<ID3D12DescriptorHeap>		_cbvHeap;  //루트테이블
	D3D12_CPU_DESCRIPTOR_HANDLE			_cpuHandleBegin = {};  //시작핸들 주소
	uint32								_handleIncrementSize = 0;  //몇칸을 띄워야 다음 핸들로 넘어갈수있는지.

	uint32					_currentIndex = 0;  //어디까지 사용했는지 추적 (모든게 다 그려졌으면 다시 0부터 시작해서 동작한다고 생각하면 됌)

	CBV_REGISTER			_reg = {};

};

