#pragma once

class Scene;

enum
{
	MAX_LAYER = 32  //���̾��� ������ �ּ� ī�޶󸶴� ���̾���ġ�ϴ°Ÿ� �ﵵ��
};



class SceneManager
{
	DECLARE_SINGLE(SceneManager);

public:
	void Update();
	void Render();
	void LoadScene(wstring sceneName);  //���̸��� �޾Ƽ� �ش��ϴ� ���� �ε�

	//���̾�κ�
	void SetLayerName(uint8 index, const wstring& name);
	const wstring& IndexToLayerName(uint8 index) { return _layerNames[index]; }
	uint8 LayerNameToIndex(const wstring& name);

	shared_ptr<class GameObject> Pick(int32 screenX, int32 screenY);


public:
	shared_ptr<Scene> GetActiveScene() { return _activeScene; }

private:
	shared_ptr<Scene> LoadTestScene();

private:
	shared_ptr<Scene> _activeScene;  //���� �����ִ� ���� �������

	//�ϴ� ���⼭ ���̾� ����
	array<wstring, MAX_LAYER> _layerNames;  //�̸�����
	map<wstring, uint8> _layerIndex;

};

