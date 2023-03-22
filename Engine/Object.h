#pragma once

enum class OBJECT_TYPE : uint8
{
	NONE,
	GAMEOBJECT, // Prefab
	COMPONENT,
	MATERIAL,
	MESH,
	MESH_DATA,
	SHADER,
	TEXTURE,

	END
};

enum
{
	OBJECT_TYPE_COUNT = static_cast<uint8>(OBJECT_TYPE::END)
};


class Object
{
public:
	Object(OBJECT_TYPE type);
	virtual ~Object();

	OBJECT_TYPE GetType() { return _objectType; }  //오브젝트 타입

	void SetName(const wstring& name) { _name = name; }
	const wstring& GetName() { return _name; }

	uint32 GetID() { return _id; }  //내가 만들어준 오브젝트의 아이디가 몇인지 관리

	// TODO : Instantiate

protected:
	friend class Resources; 
	virtual void Load(const wstring& path) { }  //저장하고 불러오기
	virtual void Save(const wstring& path) { }

protected:
	OBJECT_TYPE _objectType = OBJECT_TYPE::NONE;
	wstring _name;

protected:
	uint32 _id = 0;

};

