#include "pch.h"
#include "Engine.h"

void SwapChain::Init(const WindowInfo& info, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue)
{

	
	CreateSwapChain(info, dxgi, cmdQueue);
	//CreateRTV(device);

}

void SwapChain::Present()
{
	// Present the frame.
	_swapChain->Present(0, 0);  //현재 화면에 그려주는 역할
}

void SwapChain::SwapIndex()
{
	_backBufferIndex = (_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
}


void SwapChain::CreateSwapChain(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue)
{
	// 이전에 만든 정보 날린다 
	//혹시나 Init 두번 했을까봐 해주는 부분임
	_swapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	//화면의 크기와 같게 만들어야 함.
	sd.BufferDesc.Width = static_cast<uint32>(info.width); // 버퍼의 해상도 너비
	sd.BufferDesc.Height = static_cast<uint32>(info.height); // 버퍼의 해상도 높이
	sd.BufferDesc.RefreshRate.Numerator = 60; // 화면 갱신 비율
	sd.BufferDesc.RefreshRate.Denominator = 1; // 화면 갱신 비율
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 버퍼의 디스플레이 형식
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1; // 멀티 샘플링 OFF
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 후면 버퍼에 렌더링할 것 
	sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT; // 전면+후면 버퍼  2개 EnginePch 2
	sd.OutputWindow = info.hwnd;
	sd.Windowed = info.windowed;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 전면 후면 버퍼 교체 시 이전 프레임 정보 버림
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//dxgi를 통해 초기화 됌. ->화면에 관련된 기능
	dxgi->CreateSwapChain(cmdQueue.Get(), &sd, &_swapChain);

//	//저장하는중임
//	for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
//		_swapChain->GetBuffer(i, IID_PPV_ARGS(&_rtvBuffer[i]));
}

#pragma region CreateRTV_주석
//void SwapChain::CreateRTV(ComPtr<ID3D12Device> device)
//{
//
//	// Descriptor (DX12) = View (~DX11)
//	// [서술자 힙]으로 RTV 생성
//	// DX11의 RTV(RenderTargetView), DSV(DepthStencilView), 
//	// CBV(ConstantBufferView), SRV(ShaderResourceView), UAV(UnorderedAccessView)를 전부!
//	//걍 버전 올라오면서 얘가 저 위에거 다 관리한다는걸 저렇게 주석으로 써놓은거임.
//
//	//렌더 타겟 뷰의 사이즈를 계산함.
//	int32 _rtvHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//
//	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
//	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;  //타입
//	rtvDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;  //갯수
//	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//	rtvDesc.NodeMask = 0;
//
//	// 같은 종류의 데이터끼리 배열로 관리
//	// RTV 목록 : [ ] [ ]   //이렇게 일종의 배열로 관리되고 있음.
//	// DSC 목록 : [ ] [ ] [ ] //지워졌다...ㅋ
//	device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&_rtvHeap));  //두개 짜리의 정보를 담을수있는 배열이 만들어진거임.
//																	  //얘가 배열의 시작주소를 가리키고 있다고 생각하면 됌
//	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
//
//	//두개를 만들었으니까 지금 루프 두개 해주면서 저위에 배열안에 데이터를 참조해서 뷰를 만들어줌
//	for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
//	{
//		//CD가 붙어있는거는 d3dx12에있는거임.
//		//어떤 객체를 가리키고 있는 정수라고 볼수도 있는거임.
//		//핸들을 통해서 어떤건지 바로 알수있음.
//		_rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * _rtvHeapSize);  //이게 지금 저 위에 rtvHeapBegin이거가
//		//배열 첫번째 [0]을 가리키고 있는건데, 이거를 이제 [0], [1] 이렇게 i에 따라 배열 방 뒤로 가고있는거임.
//		//렌더 타겟은 스왑체인에있었음. 이거를 스왑체인에서 꺼내와서 렌더타겟뷰를 만들어주고있음
//		device->CreateRenderTargetView(_rtvBuffer[i].Get(), nullptr, _rtvHandle[i]);
//	}
//
//}
#pragma endregion

