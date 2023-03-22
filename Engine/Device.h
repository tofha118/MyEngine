#pragma once

//�η»繫��
// GPU�� ������ ģ�� ���� ��ü ������ ���
//���� ������������ ��� ���� ����ϵ��� �ƾ��µ� ���� ���� �ö󰡸鼭 ����� �� �ɰ���

class Device
{
public:

	void Init();

	ComPtr<IDXGIFactory> GetDXGI() { return _dxgi; }
	ComPtr<ID3D12Device> GetDevice() { return _device; }

private:
	// COM(Component Object Model)
// - DX�� ���α׷��� ��� �������� ���� ȣȯ���� �����ϰ� �ϴ� ���
// - COM ��ü(COM �������̽�)�� ���. ���λ����� �츮���� ������
// - ComPtr ������ ����Ʈ ������
	//�̰ŷ� GPU�ѱ�鼭 �۵� ��ų���ִ�
	//Com��ü�� ����� �� �����Ҵ����� ����ϴ°� �ƴ϶� ������ش����� ����Ʈ ������ ���� �ذ� ��Ƶ���.
	//���۷��� ī��Ʈ�� 0�� �Ǹ� �ڵ����� �����.
	ComPtr<ID3D12Debug>			_debugController;
	ComPtr<IDXGIFactory>		_dxgi; // ȭ�� ���� ��ɵ�
	ComPtr<ID3D12Device>		_device; // ���� ��ü ����

	

};
