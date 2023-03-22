#pragma once


#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"


class Resources
{
	DECLARE_SINGLE(Resources);

public:
	void Init();

	template<typename T>
	shared_ptr<T> Load(const wstring& key, const wstring& path);

	template<typename T>
	bool Add(const wstring& key, shared_ptr<T> object);

	template<typename T>
	shared_ptr<T> Get(const wstring& Key);

	template<typename T>
	OBJECT_TYPE GetObjectType();

	//점 하나만 보내기
	shared_ptr<Mesh> LoadPointMesh();
	shared_ptr<Mesh> LoadRectangleMesh();
	shared_ptr<Mesh> LoadCubeMesh();
	shared_ptr<Mesh> LoadSphereMesh();
	shared_ptr<Mesh> LoadTerrainMesh(int32 sizeX = 15, int32 sizeZ = 15);


	//생성딘거를 
	shared_ptr<Texture> CreateTexture(const wstring& name, DXGI_FORMAT format, uint32 width, uint32 height,
		const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
		D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE, Vec4 clearColor = Vec4());

	shared_ptr<Texture> CreateTextureFromResource(const wstring& name, ComPtr<ID3D12Resource> tex2D);

	shared_ptr<class MeshData> LoadFBX(const wstring& path);

private:
	void CreateDefaultShader();
	void CreateDefaultMaterial();


private:
	using KeyObjMap = std::map<wstring/*key*/, shared_ptr<Object>>;  //게임오브젝트타입에 따라 맵을 만들어줌.
	array<KeyObjMap, OBJECT_TYPE_COUNT> _resources;  //배열이지만 맵으로 이루어진 배열임.
};


template<typename T>
inline shared_ptr<T> Resources::Load(const wstring& key, const wstring& path) //파일의 이름, 경로
{
	OBJECT_TYPE objectType = GetObjectType<T>();  //어떤 오브젝트 타입인지 추출
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];  //리소스 배열에서 추출

	auto findIt = keyObjMap.find(key);  //키가 있으면 이미 있는거 뱉음
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	shared_ptr<T> object = make_shared<T>();  //없다면 
	object->Load(path);  //로드
	keyObjMap[key] = object;

	return object;
}

template<typename T>
bool Resources::Add(const wstring& key, shared_ptr<T> object)  //만들어준것을 등록
{
	OBJECT_TYPE objectType = GetObjectType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return false;

	keyObjMap[key] = object;

	return true;
}

template<typename T>
shared_ptr<T> Resources::Get(const wstring& key)
{
	OBJECT_TYPE objectType = GetObjectType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	return nullptr;
}

template<typename T>
inline OBJECT_TYPE Resources::GetObjectType()  //클래스를 컴파일 타이밍에 검사를 함.
{
	if (std::is_same_v<T, GameObject>)  //게임오브젝트면 게임오브젝트 반환
		return OBJECT_TYPE::GAMEOBJECT;
	else if (std::is_same_v<T, Material>)
		return OBJECT_TYPE::MATERIAL;
	else if (std::is_same_v<T, Mesh>)
		return OBJECT_TYPE::MESH;
	else if (std::is_same_v<T, Shader>)
		return OBJECT_TYPE::SHADER;
	else if (std::is_same_v<T, Texture>)
		return OBJECT_TYPE::TEXTURE;
	else if (std::is_convertible_v<T, Component>)
		return OBJECT_TYPE::COMPONENT;
	else
		return OBJECT_TYPE::NONE;
}