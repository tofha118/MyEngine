#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "Light.h"
#include "Engine.h"
#include "Resources.h"

void Scene::Awake()
{

	//���۷����� �ܾ���� ���۷��� ī���Ͱ� �������� ����. ���������� �� �� ����� ��.
	//shared_ptr�� ���� �Ҷ����� ���� Ƚ���� �����ϱ� ����.
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)  //���ӿ�����Ʈ ���� �Ÿ� �ϳ��� ��ȸ�ϰڴٴ� ��. ���ͳ� ����Ʈ���� �������� �� �ֵ��� �̷��� ����� ������.
	{
		gameObject->Awake();
	}
}

void Scene::Start()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Start();
	}
}

void Scene::Update()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Update();
	}
}

void Scene::LateUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->LateUpdate();
	}
}

void Scene::FinalUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->FinalUpdate();
	}
}


shared_ptr<Camera> Scene::GetMainCamera()
{
	if (_cameras.empty())
		return nullptr;

	return _cameras[0];
}

void Scene::Render()
{
	//����� ���õ� �κ��� ���⿡ �־���
	PushLightData();

	ClearRTV();

	RenderShadow();

	RenderDeferred();

	RenderLights();

	RenderFinal();

	RenderForward();

#pragma region ���� �帧
	//// SwapChain Group �ʱ�ȭ
	//int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	//GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);
	//// Deferred Group �ʱ�ȭ
	//GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView();
	//// Lighting Group �ʱ�ȭ
	//GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->ClearRenderTargetView();

	//// Deferred OMSet
	//GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets();

	////���� ���¾ָ� �������� ����
	//shared_ptr<Camera> mainCamera = _cameras[0];
	//mainCamera->SortGameObject();
	//mainCamera->Render_Deferred();

	//GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)-\>WaitTargetToResource();

	//RenderLights();
	//GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->WaitTargetToResource();

	//RenderFinal();

	//mainCamera->Render_Forward();

	//for (auto& camera : _cameras)
	//{
	//	if (camera == mainCamera)
	//		continue;

	//	camera->SortGameObject();
	//	camera->Render_Forward();
	//}
#pragma endregion

}

void Scene::ClearRTV()
{
	// SwapChain Group �ʱ�ȭ
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);
	// Shadow Group �ʱ�ȭ
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->ClearRenderTargetView();
	// Deferred Group �ʱ�ȭ
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView();
	// Lighting Group �ʱ�ȭ
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->ClearRenderTargetView();
}


void Scene::RenderShadow()
{
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->OMSetRenderTargets();

	for (auto& light : _lights)  //��� ����Ʈ üũ ���ָ鼭 �𷺼ų� ����Ʈ�� ��쿡�� �׸��� �׷���.
	{
		if (light->GetLightType() != LIGHT_TYPE::DIRECTIONAL_LIGHT)
			continue;

		light->RenderShadow();
	}

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->WaitTargetToResource();
}

void Scene::RenderDeferred()
{
	// Deferred OMSet
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets();

	shared_ptr<Camera> mainCamera = _cameras[0];
	mainCamera->SortGameObject();
	mainCamera->Render_Deferred();

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->WaitTargetToResource();
}

void Scene::RenderLights()
{
	shared_ptr<Camera> mainCamera = _cameras[0];  //���� ī�޶� �̾ƿ���
	Camera::S_MatView = mainCamera->GetViewMatrix();  //�� ��Ʈ����
	Camera::S_MatProjection = mainCamera->GetProjectionMatrix();  //�������� ��Ʈ����

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->OMSetRenderTargets();

	// ������ �׸���.
	for (auto& light : _lights)
	{
		light->Render();
	}

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->WaitTargetToResource();
}




void Scene::RenderFinal()
{
	// Swapchain OMSet
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTargets(1, backIndex);

	GET_SINGLE(Resources)->Get<Material>(L"Final")->PushGraphicsData();
	GET_SINGLE(Resources)->Get<Mesh>(L"Rectangle")->Render();
}

void Scene::RenderForward()
{
	shared_ptr<Camera> mainCamera = _cameras[0];
	mainCamera->Render_Forward();

	for (auto& camera : _cameras)
	{
		if (camera == mainCamera)
			continue;

		camera->SortGameObject();
		camera->Render_Forward();
	}
}

void Scene::PushLightData()
{
	LightParams lightParams = {};

	for (auto& light : _lights)
	{
		//if (gameObject->GetLight() == nullptr)  //����Ʈ�� ����ִ� ���� �˻�
		//	continue;

		//const LightInfo& lightInfo = gameObject->GetLight()->GetLightInfo();  //����Ʈ ���� ��������
		const LightInfo& lightInfo = light->GetLightInfo();

		light->SetLightIndex(lightParams.lightCount);  //���� ���° ���̾��ٴ°� ������

		//����Ʈ ������ �ϳ��� ���� ä��
		lightParams.lights[lightParams.lightCount] = lightInfo;  //������.
		lightParams.lightCount++;
	}
	//������ �޾Ҵ� ������ ���� �־���. b0 �� ������ ����
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::GLOBAL)->SetGraphicsGlobalData(&lightParams, sizeof(lightParams));
}


void Scene::AddGameObject(shared_ptr<GameObject> gameObject)
{
	if (gameObject->GetCamera() != nullptr)
	{
		_cameras.push_back(gameObject->GetCamera());
	}
	else if (gameObject->GetLight() != nullptr)
	{
		_lights.push_back(gameObject->GetLight());
	}

	_gameObjects.push_back(gameObject);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
	if (gameObject->GetCamera())
	{
		auto findIt = std::find(_cameras.begin(), _cameras.end(), gameObject->GetCamera());
		if (findIt != _cameras.end())
			_cameras.erase(findIt);
	}
	else if (gameObject->GetLight())
	{
		auto findIt = std::find(_lights.begin(), _lights.end(), gameObject->GetLight());
		if (findIt != _lights.end())
			_lights.erase(findIt);
	}

	auto findIt = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
	if (findIt != _gameObjects.end())
	{
		_gameObjects.erase(findIt);
	}
}