#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "Engine.h"

// ************************
// GraphicsCommandQueue
// ************************

GraphicsCommandQueue::~GraphicsCommandQueue()
{
	::CloseHandle(_fenceEvent);  //�̺�Ʈ�� Ȱ�������� �̺�Ʈ ���ִ� �۾�
}

void GraphicsCommandQueue::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain)
{

	_swapChain = swapChain;

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));

	// - D3D12_COMMAND_LIST_TYPE_DIRECT : GPU�� ���� �����ϴ� ��� ���
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));

	// GPU�� �ϳ��� �ý��ۿ����� 0����
	// DIRECT or BUNDLE
	// Allocator
	// �ʱ� ���� (�׸��� ����� nullptr ����)
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));

	// CommandList�� Close / Open ���°� �ִµ�
	// Open ���¿��� Command�� �ִٰ� Close�� ���� �����ϴ� ����
	_cmdList->Close();

	//���ҽ� Ŀ�ǵ� ����Ʈ �ʱ�ȭ
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_resCmdAlloc));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _resCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_resCmdList));


	// CreateFence
	// - CPU�� GPU�� ����ȭ �������� ���δ�
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);


}

void GraphicsCommandQueue::WaitSync()
{

	// Advance the fence value to mark commands up to this fence point.
	_fenceValue++;  //�ϰ��� ó���ϰ� ���� ��ȣ�� �ް��ִ°���. �׷��� ���� ��Ŵ

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	_cmdQueue->Signal(_fence.Get(), _fenceValue);  //Ŀ�ǵ�ť���� ��ȣ�� �ǳ��ְ�

	// Wait until the GPU has completed commands up to this fence point.
	if (_fence->GetCompletedValue() < _fenceValue)
	{
		// Fire event when GPU hits current fence.  
		_fence->SetEventOnCompletion(_fenceValue, _fenceEvent);  //fence������ �� ��ȣ���� �������� �̺�Ʈ�� �Ѷ�

		// Wait until the GPU hits current fence event is fired.
		::WaitForSingleObject(_fenceEvent, INFINITE);  //�ش� �̺�Ʈ�� ���������� ����ϴ¹��
	}

}


//�ٲ�ġ�� �Դٰ��� �ϴºκ��� GPU���� ���� ��������� �˷��ְ� �۾� ��û�ϴ� �κ���
void GraphicsCommandQueue::RenderBegin()
{
	//���� Ŭ����� ����ϴ�
	_cmdAlloc->Reset();
	_cmdList->Reset(_cmdAlloc.Get(), nullptr);

	int8 backIndex = _swapChain->GetBackBufferIndex();

	//CD�����Ŵ� ���� ���� Ŭ�����ε�(d3dx12) �갡 �ϰ��ִ°�  ���� ���� �����̶�� �����ϸ� ��.
	//���� ����۸��ҽ��� �̵����Ѽ� �¸� GPU�۾��뵵�� ����ڴٴ°���.
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		//_swapChain->GetBackRTVBuffer().Get(),  //� ���ҽ��� �޾��ְ�����.
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTex2D().Get(), 
		D3D12_RESOURCE_STATE_PRESENT, // ȭ�� ���  
		D3D12_RESOURCE_STATE_RENDER_TARGET); // ���� �����


	_cmdList->SetGraphicsRootSignature(GRAPHICS_ROOT_SIGNATURE.Get());  //�����ϴºκ�
//	GEngine->GetCB()->Clear(); //�׸��� �����ϴ� �κп����� ���� �ʱ�ȭ�ϰ� �ٽ� �׸��°Ŵϱ� ���۵� �ʱ�ȭ
	GEngine->GetConstantBuffer(CONSTANT_BUFFER_TYPE::TRANSFORM)->Clear();
	GEngine->GetConstantBuffer(CONSTANT_BUFFER_TYPE::MATERIAL)->Clear();

	GEngine->GetGraphicsDescHeap()->Clear();  //���̺�


	ID3D12DescriptorHeap* descHeap = GEngine->GetGraphicsDescHeap()->GetDescriptorHeap().Get();
	_cmdList->SetDescriptorHeaps(1, &descHeap);  //���� ��� ����Ұ����� ���� ���� �ؾ��Ѵ�


	//���� �� ������(���� �����) �о�ִ°���
	_cmdList->ResourceBarrier(1, &barrier);

	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
	//_cmdList->RSSetViewports(1, vp);  //������ �о����� �� ��������� �ȴٱ׷�.
	//_cmdList->RSSetScissorRects(1, rect);  ���� �̰� ���⼭ ���ϰ� rendertargetGroup���� �Ұž�

	// Specify the buffers we are going to render to.
	//_descHeap���� ����۸� �����´����� GPU�� ������� �ϰ��� �׷��޶�� ��û�ϴ�����. (����۸� GPU���� �˷��ִ°���)
	//D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _swapChain->GetBackRTV();
	//Ŭ���� �ϴ°ǵ� �ƹ��͵� ���»��¿��� LightSteelBlue(���� �ϴû�) �� �� ���� �����ΰ���.
	//_cmdList->ClearRenderTargetView(backBufferView, Colors::Black, 0, nullptr);
	//_cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, nullptr);

	//D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = GEngine->GetDepthStencilBuffer()->GetDSVCpuHandle();
	//_cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, &depthStencilView); 

	//_cmdList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

}

void GraphicsCommandQueue::RenderEnd()
{
	int8 backIndex = _swapChain->GetBackBufferIndex();

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTex2D().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, // ���� �����    //���� RenderBegin�� ����ѵ� ������ �ٸ�.
		//���� ������� ȭ����� �뵵�� �ٲ�ġ�� ȭ�� ����� �ٽ� �����(���� �����)�� �ٲٴ°���
		D3D12_RESOURCE_STATE_PRESENT); // ȭ�� ���

	//������ �ϰ� �о�ִ°���
	_cmdList->ResourceBarrier(1, &barrier);
	_cmdList->Close();  //����Ʈ �ݾ���. (�� �ִ°� �ٳ����ٰ� �˷��ִ°���)

	//���� ���������� ���� �� �о�ִ°ſ���.
	//��¥ �� ��û�� �ؿ��� �ϴ°���
	// Ŀ�ǵ� ����Ʈ ����
	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	//�� ���۷� ȭ�� ä���ִ� �κ�
	_swapChain->Present();

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	//������ ��û�� �۾��� �� �Ϸᰡ �� �� ���� CPU�� ��ٸ������ΰ���.
	WaitSync();

	//������ �ִ� �� ���۸� �ٲ�ġ�� �ϴ°���. (ȭ�鿡 �� ���� ����� �غ� ��ģ����)
	_swapChain->SwapIndex();
}


void GraphicsCommandQueue::FlushResourceCommandQueue()
{
	//���ҽ� Ŀ�ǵ� ����Ʈ�� �ݰ�
	_resCmdList->Close();

	//���� �����ش�����
	ID3D12CommandList* cmdListArr[] = { _resCmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);
	//��ٸ�
	WaitSync();
	//����
	_resCmdAlloc->Reset();
	_resCmdList->Reset(_resCmdAlloc.Get(), nullptr);
}



// ************************
// ComputeCommandQueue
// ************************

//���������� �ϴ� ���� ����ѵ� �����Ҷ� ���ڰ� �ٸ�.. �̰� �ٸ��� �۵��� ���Ҷ��� ���Ƽ� �����ؾ���
ComputeCommandQueue::~ComputeCommandQueue()
{
	::CloseHandle(_fenceEvent);
}

void ComputeCommandQueue::Init(ComPtr<ID3D12Device> device)
{
	D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
	computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;  //Ÿ���� �׷���Ŀ�ǵ�� �ٸ�. 
	computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	device->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&_cmdQueue));

	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&_cmdAlloc));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));

	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

	// CreateFence
	// - CPU�� GPU�� ����ȭ �������� ���δ�
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void ComputeCommandQueue::WaitSync()
{
	_fenceValue++;

	_cmdQueue->Signal(_fence.Get(), _fenceValue);

	if (_fence->GetCompletedValue() < _fenceValue)
	{
		_fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
		::WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

void ComputeCommandQueue::FlushComputeCommandQueue()
{
	_cmdList->Close();

	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	auto t = _countof(cmdListArr);
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);  //�ϰ� �����ֱ�

	WaitSync();

	_cmdAlloc->Reset();
	_cmdList->Reset(_cmdAlloc.Get(), nullptr);

	COMPUTE_CMD_LIST->SetComputeRootSignature(COMPUTE_ROOT_SIGNATURE.Get());
}