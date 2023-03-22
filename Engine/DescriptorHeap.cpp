#include "pch.h"
#include "DescriptorHeap.h"
#include "SwapChain.h"

void DescriptorHeap::Init(ComPtr<ID3D12Device> device, shared_ptr< SwapChain> swapChain)
{
	_swapChain = swapChain;  //�����ִ� ����ü���� �����ؼ� �԰ݼ�(DescriptorHeap)�� �־��ִ°ǵ�, �̰� �������°Ű��ƺ�������
	//���⼭ �̷��� �϶�� ...�Ǿ����������׷��� �ϱ�Ⱦ �ؾߴ碌��

	// Descriptor (DX12) = View (~DX11)
	// [������ ��]���� RTV ����
	// DX11�� RTV(RenderTargetView), DSV(DepthStencilView), 
	// CBV(ConstantBufferView), SRV(ShaderResourceView), UAV(UnorderedAccessView)�� ����!
	//�� ���� �ö���鼭 �갡 �� ������ �� �����Ѵٴ°� ������ �ּ����� ���������.

	//���� Ÿ�� ���� ����� �����.
	_rtvHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;  //Ÿ��
	rtvDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;  //����
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDesc.NodeMask = 0;

	// ���� ������ �����ͳ��� �迭�� ����
	// RTV ��� : [ ] [ ]   //�̷��� ������ �迭�� �����ǰ� ����.
	// DSC ��� : [ ] [ ] [ ] //��������...��
	device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&_rtvHeap));  //�ΰ� ¥���� ������ �������ִ� �迭�� �����������.

																	  //�갡 �迭�� �����ּҸ� ����Ű�� �ִٰ� �����ϸ� ��
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();

	//�ΰ��� ��������ϱ� ���� ���� �ΰ� ���ָ鼭 ������ �迭�ȿ� �����͸� �����ؼ� �並 �������
	for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
	{
		//CD�� �پ��ִ°Ŵ� d3dx12���ִ°���.
		//� ��ü�� ����Ű�� �ִ� ������� ������ �ִ°���.
		//�ڵ��� ���ؼ� ����� �ٷ� �˼�����.
		_rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * _rtvHeapSize);  //�̰� ���� �� ���� rtvHeapBegin�̰Ű�
		//�迭 ù��° [0]�� ����Ű�� �ִ°ǵ�, �̰Ÿ� ���� [0], [1] �̷��� i�� ���� �迭 �� �ڷ� �����ִ°���.
		//���� Ÿ���� ����ü�ο��־���. �̰Ÿ� ����ü�ο��� �����ͼ� ����Ÿ�ٺ並 ������ְ�����
		device->CreateRenderTargetView(swapChain->GetRenderTarget(i).Get(), nullptr, _rtvHandle[i]);
	}

}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetBackBufferView()
{
	//����ü�ο��� ������ε����� �����ؼ� �װ��� RTV�� ��ȯ���ִ°���.
	return GetRTV(_swapChain->GetCurrentBackBufferIndex());

}
