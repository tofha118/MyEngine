#pragma once

class Scene;

enum
{
	MAX_LAYER = 32  //레이어라는 개념을 둬서 카메라마다 레이어일치하는거만 찍도록
};



class SceneManager
{
	DECLARE_SINGLE(SceneManager);

public:
	void Update();
	void Render();
	void LoadScene(wstring sceneName);  //씬이름을 받아서 해당하는 씬을 로드

	//레이어부분
	void SetLayerName(uint8 index, const wstring& name);
	const wstring& IndexToLayerName(uint8 index) { return _layerNames[index]; }
	uint8 LayerNameToIndex(const wstring& name);

	shared_ptr<class GameObject> Pick(int32 screenX, int32 screenY);


public:
	shared_ptr<Scene> GetActiveScene() { return _activeScene; }

private:
	shared_ptr<Scene> LoadTestScene();

private:
	shared_ptr<Scene> _activeScene;  //현재 켜져있는 씬을 들고있음

	//일단 여기서 레이어 관리
	array<wstring, MAX_LAYER> _layerNames;  //이름매핑
	map<wstring, uint8> _layerIndex;

};

