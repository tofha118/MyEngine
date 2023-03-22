#include "pch.h"
#include "DescriptorHeap.h"
#include "SwapChain.h"

void DescriptorHeap::Init(ComPtr<ID3D12Device> device, shared_ptr< SwapChain> swapChain)
{
	_swapChain = swapChain;  //원래있던 스왑체인을 복사해서 규격서(DescriptorHeap)에 넣어주는건데, 이게 쓸데없는거같아보이지만
	//여기서 이렇게 하라고 ...되어있음ㅋㅋ그래서 하기싫어도 해야댐ㅋㅋ

	// Descriptor (DX12) = View (~DX11)
	// [서술자 힙]으로 RTV 생성
	// DX11의 RTV(RenderTargetView), DSV(DepthStencilView), 
	// CBV(ConstantBufferView), SRV(ShaderResourceView), UAV(UnorderedAccessView)를 전부!
	//걍 버전 올라오면서 얘가 저 위에거 다 관리한다는걸 저렇게 주석으로 써놓은거임.

	//렌더 타겟 뷰의 사이즈를 계산함.
	_rtvHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;  //타입
	rtvDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;  //갯수
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDesc.NodeMask = 0;

	// 같은 종류의 데이터끼리 배열로 관리
	// RTV 목록 : [ ] [ ]   //이렇게 일종의 배열로 관리되고 있음.
	// DSC 목록 : [ ] [ ] [ ] //지워졌다...ㅋ
	device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&_rtvHeap));  //두개 짜리의 정보를 담을수있는 배열이 만들어진거임.

																	  //얘가 배열의 시작주소를 가리키고 있다고 생각하면 됌
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();

	//두개를 만들었으니까 지금 루프 두개 해주면서 저위에 배열안에 데이터를 참조해서 뷰를 만들어줌
	for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
	{
		//CD가 붙어있는거는 d3dx12에있는거임.
		//어떤 객체를 가리키고 있는 정수라고 볼수도 있는거임.
		//핸들을 통해서 어떤건지 바로 알수있음.
		_rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * _rtvHeapSize);  //이게 지금 저 위에 rtvHeapBegin이거가
		//배열 첫번째 [0]을 가리키고 있는건데, 이거를 이제 [0], [1] 이렇게 i에 따라 배열 방 뒤로 가고있는거임.
		//렌더 타겟은 스왑체인에있었음. 이거를 스왑체인에서 꺼내와서 렌더타겟뷰를 만들어주고있음
		device->CreateRenderTargetView(swapChain->GetRenderTarget(i).Get(), nullptr, _rtvHandle[i]);
	}

}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetBackBufferView()
{
	//스왑체인에서 백버퍼인덱스를 참조해서 그것의 RTV를 반환해주는거임.
	return GetRTV(_swapChain->GetCurrentBackBufferIndex());

}
