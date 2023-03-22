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
	_swapChain->Present(0, 0);  //���� ȭ�鿡 �׷��ִ� ����
}

void SwapChain::SwapIndex()
{
	_backBufferIndex = (_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
}


void SwapChain::CreateSwapChain(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue)
{
	// ������ ���� ���� ������ 
	//Ȥ�ó� Init �ι� ������� ���ִ� �κ���
	_swapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	//ȭ���� ũ��� ���� ������ ��.
	sd.BufferDesc.Width = static_cast<uint32>(info.width); // ������ �ػ� �ʺ�
	sd.BufferDesc.Height = static_cast<uint32>(info.height); // ������ �ػ� ����
	sd.BufferDesc.RefreshRate.Numerator = 60; // ȭ�� ���� ����
	sd.BufferDesc.RefreshRate.Denominator = 1; // ȭ�� ���� ����
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ������ ���÷��� ����
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1; // ��Ƽ ���ø� OFF
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �ĸ� ���ۿ� �������� �� 
	sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT; // ����+�ĸ� ����  2�� EnginePch 2
	sd.OutputWindow = info.hwnd;
	sd.Windowed = info.windowed;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // ���� �ĸ� ���� ��ü �� ���� ������ ���� ����
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//dxgi�� ���� �ʱ�ȭ ��. ->ȭ�鿡 ���õ� ���
	dxgi->CreateSwapChain(cmdQueue.Get(), &sd, &_swapChain);

//	//�����ϴ�����
//	for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
//		_swapChain->GetBuffer(i, IID_PPV_ARGS(&_rtvBuffer[i]));
}

#pragma region CreateRTV_�ּ�
//void SwapChain::CreateRTV(ComPtr<ID3D12Device> device)
//{
//
//	// Descriptor (DX12) = View (~DX11)
//	// [������ ��]���� RTV ����
//	// DX11�� RTV(RenderTargetView), DSV(DepthStencilView), 
//	// CBV(ConstantBufferView), SRV(ShaderResourceView), UAV(UnorderedAccessView)�� ����!
//	//�� ���� �ö���鼭 �갡 �� ������ �� �����Ѵٴ°� ������ �ּ����� ���������.
//
//	//���� Ÿ�� ���� ����� �����.
//	int32 _rtvHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//
//	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
//	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;  //Ÿ��
//	rtvDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;  //����
//	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//	rtvDesc.NodeMask = 0;
//
//	// ���� ������ �����ͳ��� �迭�� ����
//	// RTV ��� : [ ] [ ]   //�̷��� ������ �迭�� �����ǰ� ����.
//	// DSC ��� : [ ] [ ] [ ] //��������...��
//	device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&_rtvHeap));  //�ΰ� ¥���� ������ �������ִ� �迭�� �����������.
//																	  //�갡 �迭�� �����ּҸ� ����Ű�� �ִٰ� �����ϸ� ��
//	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
//
//	//�ΰ��� ��������ϱ� ���� ���� �ΰ� ���ָ鼭 ������ �迭�ȿ� �����͸� �����ؼ� �並 �������
//	for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
//	{
//		//CD�� �پ��ִ°Ŵ� d3dx12���ִ°���.
//		//� ��ü�� ����Ű�� �ִ� ������� ������ �ִ°���.
//		//�ڵ��� ���ؼ� ����� �ٷ� �˼�����.
//		_rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * _rtvHeapSize);  //�̰� ���� �� ���� rtvHeapBegin�̰Ű�
//		//�迭 ù��° [0]�� ����Ű�� �ִ°ǵ�, �̰Ÿ� ���� [0], [1] �̷��� i�� ���� �迭 �� �ڷ� �����ִ°���.
//		//���� Ÿ���� ����ü�ο��־���. �̰Ÿ� ����ü�ο��� �����ͼ� ����Ÿ�ٺ並 ������ְ�����
//		device->CreateRenderTargetView(_rtvBuffer[i].Get(), nullptr, _rtvHandle[i]);
//	}
//
//}
#pragma endregion

