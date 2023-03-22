#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "Engine.h"

// ************************
// GraphicsCommandQueue
// ************************

GraphicsCommandQueue::~GraphicsCommandQueue()
{
	::CloseHandle(_fenceEvent);  //이벤트를 활용했으면 이벤트 꺼주는 작업
}

void GraphicsCommandQueue::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain)
{

	_swapChain = swapChain;

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));

	// - D3D12_COMMAND_LIST_TYPE_DIRECT : GPU가 직접 실행하는 명령 목록
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));

	// GPU가 하나인 시스템에서는 0으로
	// DIRECT or BUNDLE
	// Allocator
	// 초기 상태 (그리기 명령은 nullptr 지정)
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));

	// CommandList는 Close / Open 상태가 있는데
	// Open 상태에서 Command를 넣다가 Close한 다음 제출하는 개념
	_cmdList->Close();

	//리소스 커맨드 리스트 초기화
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_resCmdAlloc));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _resCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_resCmdList));


	// CreateFence
	// - CPU와 GPU의 동기화 수단으로 쓰인다
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);


}

void GraphicsCommandQueue::WaitSync()
{

	// Advance the fence value to mark commands up to this fence point.
	_fenceValue++;  //일감을 처리하고 나서 번호를 메겨주는거임. 그러고 증가 시킴

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	_cmdQueue->Signal(_fence.Get(), _fenceValue);  //커맨드큐에도 번호를 건네주고

	// Wait until the GPU has completed commands up to this fence point.
	if (_fence->GetCompletedValue() < _fenceValue)
	{
		// Fire event when GPU hits current fence.  
		_fence->SetEventOnCompletion(_fenceValue, _fenceEvent);  //fence에서도 이 번호까지 끝났으면 이벤트를 켜라

		// Wait until the GPU hits current fence event is fired.
		::WaitForSingleObject(_fenceEvent, INFINITE);  //해당 이벤트가 켜질때까지 대기하는방식
	}

}


//바꿔치기 왔다갔다 하는부분임 GPU한테 뭐가 백버퍼인지 알려주고 작업 요청하는 부분임
void GraphicsCommandQueue::RenderBegin()
{
	//벡터 클리어랑 비슷하다
	_cmdAlloc->Reset();
	_cmdList->Reset(_cmdAlloc.Get(), nullptr);

	int8 backIndex = _swapChain->GetBackBufferIndex();

	//CD붙은거는 이제 헬퍼 클래스인디(d3dx12) 얘가 하고있는게  지금 버퍼 설정이라고 생각하면 됌.
	//현재 백버퍼리소스를 이동시켜서 걔를 GPU작업용도로 만들겠다는거임.
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		//_swapChain->GetBackRTVBuffer().Get(),  //어떤 리소스를 받아주고있음.
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTex2D().Get(), 
		D3D12_RESOURCE_STATE_PRESENT, // 화면 출력  
		D3D12_RESOURCE_STATE_RENDER_TARGET); // 외주 결과물


	_cmdList->SetGraphicsRootSignature(GRAPHICS_ROOT_SIGNATURE.Get());  //서명하는부분
//	GEngine->GetCB()->Clear(); //그리기 시작하는 부분에서는 모든거 초기화하고 다시 그리는거니까 버퍼도 초기화
	GEngine->GetConstantBuffer(CONSTANT_BUFFER_TYPE::TRANSFORM)->Clear();
	GEngine->GetConstantBuffer(CONSTANT_BUFFER_TYPE::MATERIAL)->Clear();

	GEngine->GetGraphicsDescHeap()->Clear();  //테이블


	ID3D12DescriptorHeap* descHeap = GEngine->GetGraphicsDescHeap()->GetDescriptorHeap().Get();
	_cmdList->SetDescriptorHeaps(1, &descHeap);  //내가 어떤거 사용할건지를 먼저 선언 해야한다


	//지금 저 위에거(외주 결과물) 밀어넣는거임
	_cmdList->ResourceBarrier(1, &barrier);

	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
	//_cmdList->RSSetViewports(1, vp);  //위에서 밀었으면 또 세팅해줘야 된다그럼.
	//_cmdList->RSSetScissorRects(1, rect);  이제 이거 여기서 안하고 rendertargetGroup에서 할거야

	// Specify the buffers we are going to render to.
	//_descHeap에서 백버퍼를 꺼내온다음에 GPU를 대상으로 일감을 그려달라고 요청하는중임. (백버퍼를 GPU한테 알려주는거임)
	//D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _swapChain->GetBackRTV();
	//클리어 하는건데 아무것도 없는상태에서 LightSteelBlue(연한 하늘색) 뜸 걍 색상 설정인거임.
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
		D3D12_RESOURCE_STATE_RENDER_TARGET, // 외주 결과물    //위에 RenderBegin랑 비슷한데 순서가 다름.
		//외주 결과물을 화면출력 용도로 바꿔치고 화면 출력을 다시 백버퍼(외주 결과물)로 바꾸는거임
		D3D12_RESOURCE_STATE_PRESENT); // 화면 출력

	//위에거 일감 밀어넣는거임
	_cmdList->ResourceBarrier(1, &barrier);
	_cmdList->Close();  //리스트 닫아줌. (일 넣는거 다끝났다고 알려주는거임)

	//지금 위에까지는 전부 일 밀어넣는거였음.
	//진짜 일 요청은 밑에서 하는거임
	// 커맨드 리스트 수행
	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	//앞 버퍼로 화면 채워주는 부분
	_swapChain->Present();

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	//위에서 요청한 작업이 다 완료가 될 때 까지 CPU가 기다리는중인거임.
	WaitSync();

	//기존에 있던 백 버퍼를 바꿔치기 하는거임. (화면에 백 버퍼 출력할 준비를 마친거임)
	_swapChain->SwapIndex();
}


void GraphicsCommandQueue::FlushResourceCommandQueue()
{
	//리소스 커맨드 리스트를 닫고
	_resCmdList->Close();

	//위로 날려준다음에
	ID3D12CommandList* cmdListArr[] = { _resCmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);
	//기다림
	WaitSync();
	//리셋
	_resCmdAlloc->Reset();
	_resCmdList->Reset(_resCmdAlloc.Get(), nullptr);
}



// ************************
// ComputeCommandQueue
// ************************

//실질적으로 하는 일은 비슷한데 생성할때 인자가 다름.. 이게 다르면 작동이 안할때가 많아서 조심해야함
ComputeCommandQueue::~ComputeCommandQueue()
{
	::CloseHandle(_fenceEvent);
}

void ComputeCommandQueue::Init(ComPtr<ID3D12Device> device)
{
	D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
	computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;  //타입이 그래픽커맨드랑 다름. 
	computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	device->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&_cmdQueue));

	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&_cmdAlloc));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));

	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

	// CreateFence
	// - CPU와 GPU의 동기화 수단으로 쓰인다
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
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);  //일감 날려주기

	WaitSync();

	_cmdAlloc->Reset();
	_cmdList->Reset(_cmdAlloc.Get(), nullptr);

	COMPUTE_CMD_LIST->SetComputeRootSignature(COMPUTE_ROOT_SIGNATURE.Get());
}