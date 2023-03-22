#pragma once
#include "Object.h"

class Texture : public Object
{

public:
	Texture();
	virtual ~Texture();

	virtual void Load(const wstring& path) override;  //���ҽ� �ε��ؼ� ����ϴ°�

public:  //�̰Ŵ� �ƿ� ó������ 
	void Create(DXGI_FORMAT format, uint32 width, uint32 height,
		const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
		D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor = Vec4());

	void CreateFromResource(ComPtr<ID3D12Resource> tex2D);  //���ҽ��� �̿��ؼ� ������ִ� �κ� (����ü�� �Ҷ����� ���)

public:
	ComPtr<ID3D12Resource> GetTex2D() { return _tex2D; }
	ComPtr<ID3D12DescriptorHeap> GetSRV() { return _srvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetRTV() { return _rtvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetUAV() { return _uavHeap; }
	ComPtr<ID3D12DescriptorHeap> GetDSV() { return _dsvHeap; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() { return _srvHeapBegin; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetUAVHandle() { return _uavHeapBegin; }

	float GetWidth() { return static_cast<float>(_desc.Width); }
	float GetHeight() { return static_cast<float>(_desc.Height); }
private:
	ScratchImage			 		_image;
	D3D12_RESOURCE_DESC				_desc;  //ũ�� �����ϱ� ���ؼ�
	ComPtr<ID3D12Resource>			_tex2D;

	ComPtr<ID3D12DescriptorHeap>	_srvHeap;	
	ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
	ComPtr<ID3D12DescriptorHeap>	_dsvHeap;
	ComPtr<ID3D12DescriptorHeap>	_uavHeap;  //uv0 ����Ұ�


private:
	D3D12_CPU_DESCRIPTOR_HANDLE		_uavHeapBegin = {};
	D3D12_CPU_DESCRIPTOR_HANDLE		_srvHeapBegin = {};  //uv0 ���� �ּ�

};

