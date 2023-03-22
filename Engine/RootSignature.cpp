#include "pch.h"
#include "RootSignature.h"
#include "Engine.h"


//void RootSignature::Init(ComPtr<ID3D12Device> device)
//{
//
//	CD3DX12_DESCRIPTOR_RANGE ranges[] =
//	{
//		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_COUNT, 0), // b0~b4   //어디에서 어디 쓸건지를 넘겨주는거임.
//		//텍스쳐를 밀어넣을수있음.
//		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRV_REGISTER_COUNT, 0), // t0~t4
//
//	};
//
//	CD3DX12_ROOT_PARAMETER param[1];
//	//이거는 루트테이블
//	param[0].InitAsDescriptorTable(_countof(ranges), ranges);
//
//	//param[0].InitAsConstantBufferView(0); // 0번 -> b0 -> CBV  
//	//param[1].InitAsConstantBufferView(1); // 1번 -> b1 -> CBV
//
//	//D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(D3D12_DEFAULT);
//	D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(param),param);  //원래 위에거는 아무것도 서명을 안한 상태인데,
//	//지금은 param을 매개변수로 넘겨줘서 인지를 시켜서 사용할수있게 만든거임.
//
//	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력 조립기 단계
//
//	ComPtr<ID3DBlob> blobSignature;
//	ComPtr<ID3DBlob> blobError;
//	::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
//	device->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&_signature));
//}

void RootSignature::Init()
{
//	CreateSamplerDesc();
	CreateGraphicsRootSignature();
	CreateComputeRootSignature();

}

void RootSignature::CreateGraphicsRootSignature()
{
	_samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);


	CD3DX12_DESCRIPTOR_RANGE ranges[] =
	{
		//CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_COUNT, 0), // b0~b4
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_COUNT - 1, 1), // b1~b4
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRV_REGISTER_COUNT, 0), // t0~t4
	};

	CD3DX12_ROOT_PARAMETER param[2];
	param[0].InitAsConstantBufferView(static_cast<uint32>(CBV_REGISTER::b0)); // b0 전역으로 활용할거 . (라이트처럼 한번만 세팅해도 되는것들)
	param[1].InitAsDescriptorTable(_countof(ranges), ranges);

	D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(param), param, 1, &_samplerDesc);
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력 조립기 단계

	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;
	::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
	DEVICE->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&_graphicsRootSignature));
}

void RootSignature::CreateComputeRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE ranges[] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_COUNT, 0), // b0~b4
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRV_REGISTER_COUNT, 0), // t0~t9
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, UAV_REGISTER_COUNT, 0), // u0~u4
	};

	CD3DX12_ROOT_PARAMETER param[1];
	param[0].InitAsDescriptorTable(_countof(ranges), ranges);

	D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(param), param);
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;
	::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
	DEVICE->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&_computeRootSignature));

	COMPUTE_CMD_LIST->SetComputeRootSignature(_computeRootSignature.Get());
}


//void RootSignature::CreateSamplerDesc()
//{
//	_samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);
//}