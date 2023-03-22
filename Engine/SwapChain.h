#pragma once
// ��ȯ �罽
// [���� ����]
// - ���� ���� ���� �ִ� ��Ȳ�� ����
// - � �������� ��� ������� ������
// - GPU�� ������ ��� (����)
// - ����� �޾Ƽ� ȭ�鿡 �׷��ش�

// [���� �����]�� ��� ����?
// - � ����(Buffer)�� �׷��� �ǳ��޶�� ��Ź�غ���
// - Ư�� ���̸� ���� -> ó���� �ǳ��ְ� -> ������� �ش� ���̿� �޴´� OK
// - �츮 ȭ�鿡 Ư�� ����(���� �����) ������ش�

// [?]
// - �׷��� ȭ�鿡 ���� ����� ����ϴ� ���߿�, ���� ȭ�鵵 ���ָ� �ðܾ� ��
// - ���� ȭ�� ������� �̹� ȭ�� ��¿� �����
// - Ư�� ���̸� 2�� ����, �ϳ��� ���� ȭ���� �׷��ְ�, �ϳ��� ���� �ñ��...
// - Double Buffering!

// - [0] [1]
// ���� ȭ�� [1]  <-> GPU �۾��� [1] BackBuffer

class SwapChain
{
public:

	void Init(const WindowInfo& info, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);
	void Present();
	void SwapIndex();

	ComPtr<IDXGISwapChain> GetSwapChain() { return _swapChain; }  
	uint8 GetBackBufferIndex() { return _backBufferIndex; }

//	ComPtr<ID3D12Resource> GetRenderTarget(int32 index) { return _rtvBuffer[index]; }

	//���� �����,����Ʈ �����ϴ� �ε��� ����
	//uint32 GetCurrentBackBufferIndex() { return _backBufferIndex; }
	//��� ���� ���� Ȱ���ϴ� ���� ��ü�� ���� (���� ����۰� ���������� �ǳ��ִ°���)
//	ComPtr<ID3D12Resource> GetBackRTVBuffer() { return _rtvBuffer[_backBufferIndex]; }

//	D3D12_CPU_DESCRIPTOR_HANDLE GetBackRTV() { return _rtvHandle[_backBufferIndex]; }


private:
	void CreateSwapChain(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);

private:

	ComPtr<IDXGISwapChain>	_swapChain;
//	ComPtr<ID3D12Resource>	_rtvBuffer[SWAP_CHAIN_BUFFER_COUNT];  //���� �ΰ� ����ҰŴϱ� 2�� (�׸� ���)
	uint32					_backBufferIndex = 0;  //��� ����۰� �������� �������ִ� �����. (back,front) ����


	//ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
	//D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHandle[SWAP_CHAIN_BUFFER_COUNT]; //���� ���⼭ ���� �ȸ����



};
