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

	vector<shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }  //가지고있는 모든 오브젝트를 반환


private:
	vector<shared_ptr<GameObject>> _gameObjects;
	vector<shared_ptr<class Camera>>	_cameras;
	vector<shared_ptr<class Light>>		_lights;
	//유니티처럼 레이어를 구분해서 
	//[ 0 ] wall
	// [ 1 ] object 
	//뭐 이런식으로 벡터를 여러개 만들어주면 성능 향상이 됌. 
	//왜냐하면 벡터 하나에서 1000개 이런식으로 찾는거보다 해당 레이어에서 찾는게 더 빠르자나
};

