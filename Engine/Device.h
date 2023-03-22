#pragma once

//인력사무소
// GPU와 교류할 친구 각종 객체 생성을 담당
//원래 구버전에서는 얘까 모든걸 담당하도록 됐었는데 요즘 버전 올라가면서 기능이 좀 쪼개짐

class Device
{
public:

	void Init();

	ComPtr<IDXGIFactory> GetDXGI() { return _dxgi; }
	ComPtr<ID3D12Device> GetDevice() { return _device; }

private:
	// COM(Component Object Model)
// - DX의 프로그래밍 언어 독립성과 하위 호환성을 가능하게 하는 기술
// - COM 객체(COM 인터페이스)를 사용. 세부사항은 우리한테 숨겨짐
// - ComPtr 일종의 스마트 포인터
	//이거로 GPU넘기면서 작동 시킬수있다
	//Com객체를 사용할 때 동적할당으로 사용하는게 아니라 만들어준다음에 스마트 포인터 마냥 잊고 살아도댐.
	//레퍼런스 카운트가 0이 되면 자동으로 사라짐.
	ComPtr<ID3D12Debug>			_debugController;
	ComPtr<IDXGIFactory>		_dxgi; // 화면 관련 기능들
	ComPtr<ID3D12Device>		_device; // 각종 객체 생성

	

};
