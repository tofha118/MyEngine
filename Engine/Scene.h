#pragma once

class GameObject;

class Scene
{
public:
	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();

	shared_ptr<class Camera> GetMainCamera();

	void Render();

	void ClearRTV();

	void RenderShadow();
	void RenderDeferred();
	void RenderLights();
	void RenderFinal();

	void RenderForward();

private:
	void PushLightData();

public:
	void AddGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);

	vector<shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }  //�������ִ� ��� ������Ʈ�� ��ȯ


private:
	vector<shared_ptr<GameObject>> _gameObjects;
	vector<shared_ptr<class Camera>>	_cameras;
	vector<shared_ptr<class Light>>		_lights;
	//����Ƽó�� ���̾ �����ؼ� 
	//[ 0 ] wall
	// [ 1 ] object 
	//�� �̷������� ���͸� ������ ������ָ� ���� ����� ��. 
	//�ֳ��ϸ� ���� �ϳ����� 1000�� �̷������� ã�°ź��� �ش� ���̾�� ã�°� �� �����ڳ�
};

