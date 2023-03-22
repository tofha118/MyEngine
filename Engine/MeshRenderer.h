#pragma once

#include "Component.h"

class Mesh;
class Material;
// [32][32]
union InstanceID
{
	struct
	{
		uint32 meshID;
		uint32 materialID;
	};
	uint64 id;
};
class MeshRenderer : public Component
{

public:
	MeshRenderer();
	virtual ~MeshRenderer();

	//shared_ptr<Material> GetMaterial() { return _material; }
	shared_ptr<Material> GetMaterial(uint32 idx = 0) { return _materials[idx]; }
	shared_ptr<Mesh> GetMesh() { return _mesh; }

	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material, uint32 idx = 0);

	//void SetMaterial(shared_ptr<Material> material) { _material = material; }


	void Render();  //그리는 역할
	void Render(shared_ptr<class InstancingBuffer>& buffer);  //InstancingBuffer로 렌더
	void RenderShadow();  

	uint64 GetInstanceID();


private:
	shared_ptr<Mesh> _mesh;
	//shared_ptr<Material> _material;
	vector<shared_ptr<Material>> _materials;
};

