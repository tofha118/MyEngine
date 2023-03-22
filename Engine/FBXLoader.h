#pragma once

struct FbxMaterialInfo
{
	Vec4			diffuse;  //색상
	Vec4			ambient;
	Vec4			specular;
	wstring			name;
	wstring			diffuseTexName;
	wstring			normalTexName;
	wstring			specularTexName;
};

struct BoneWeight
{
	using Pair = pair<int32, double>;  //내가 몇번째 본의 영향을 얼마 받는지
	vector<Pair> boneWeights;

	void AddWeights(uint32 index, double weight)
	{
		if (weight <= 0.f)
			return;

		auto findIt = std::find_if(boneWeights.begin(), boneWeights.end(),
			[=](const Pair& p) { return p.second < weight; });

		if (findIt != boneWeights.end())
			boneWeights.insert(findIt, Pair(index, weight));
		else
			boneWeights.push_back(Pair(index, weight));

		// 가중치는 최대 4개
		if (boneWeights.size() > 4)
			boneWeights.pop_back();
	}

	void Normalize()  //범위가 1을 초과하면 합을 1로 맞춰줌.
	{
		double sum = 0.f;
		std::for_each(boneWeights.begin(), boneWeights.end(), [&](Pair& p) { sum += p.second; });
		std::for_each(boneWeights.begin(), boneWeights.end(), [=](Pair& p) { p.second = p.second / sum; });
	}
};

struct FbxMeshInfo  //여러개의 메테리얼, 메쉬가 있을수있으니까
{
	wstring								name;
	vector<Vertex>						vertices;
	vector<vector<uint32>>				indices; //메쉬가 분리 되어서 할수있게 . (다중 메테리얼로 연동)
	vector<FbxMaterialInfo>				materials;
	vector<BoneWeight>					boneWeights; // 뼈 가중치
	bool								hasAnimation;
};

struct FbxKeyFrameInfo
{
	FbxAMatrix  matTransform;
	double		time;
};

struct FbxBoneInfo
{
	wstring					boneName;
	int32					parentIndex;
	FbxAMatrix				matOffset;
};

struct FbxAnimClipInfo
{
	wstring			name;
	FbxTime			startTime;
	FbxTime			endTime;
	FbxTime::EMode	mode;
	vector<vector<FbxKeyFrameInfo>>	keyFrames; //본에 대한 개수만큼 열, 각 행은 매 프레임마다의 정보를 나타내는 2차원 배열
};

class FBXLoader
{
public:
	FBXLoader();
	~FBXLoader();

public:
	void LoadFbx(const wstring& path);

public:
	int32 GetMeshCount() { return static_cast<int32>(_meshes.size()); }
	const FbxMeshInfo& GetMesh(int32 idx) { return _meshes[idx]; }
	vector<shared_ptr<FbxBoneInfo>>& GetBones() { return _bones; }
	vector<shared_ptr<FbxAnimClipInfo>>& GetAnimClip() { return _animClips; }
private:
	void Import(const wstring& path);

	void ParseNode(FbxNode* root);
	void LoadMesh(FbxMesh* mesh);
	void LoadMaterial(FbxSurfaceMaterial* surfaceMaterial);

	void		GetNormal(FbxMesh* mesh, FbxMeshInfo* container, int32 idx, int32 vertexCounter);
	void		GetTangent(FbxMesh* mesh, FbxMeshInfo* container, int32 idx, int32 vertexCounter);
	void		GetUV(FbxMesh* mesh, FbxMeshInfo* container, int32 idx, int32 vertexCounter);
	Vec4		GetMaterialData(FbxSurfaceMaterial* surface, const char* materialName, const char* factorName);
	wstring		GetTextureRelativeName(FbxSurfaceMaterial* surface, const char* materialProperty);

	void CreateTextures();
	void CreateMaterials();

	// Animation
	void LoadBones(FbxNode* node) { LoadBones(node, 0, -1); }
	void LoadBones(FbxNode* node, int32 idx, int32 parentIdx);
	void LoadAnimationInfo();

	void LoadAnimationData(FbxMesh* mesh, FbxMeshInfo* meshInfo);
	void LoadBoneWeight(FbxCluster* cluster, int32 boneIdx, FbxMeshInfo* meshInfo);
	void LoadOffsetMatrix(FbxCluster* cluster, const FbxAMatrix& matNodeTransform, int32 boneIdx, FbxMeshInfo* meshInfo);
	void LoadKeyframe(int32 animIndex, FbxNode* node, FbxCluster* cluster, const FbxAMatrix& matNodeTransform, int32 boneIdx, FbxMeshInfo* container);

	int32 FindBoneIndex(string name);
	FbxAMatrix GetTransform(FbxNode* node);

	void FillBoneWeight(FbxMesh* mesh, FbxMeshInfo* meshInfo);

private:
	FbxManager* _manager = nullptr;
	FbxScene* _scene = nullptr;
	FbxImporter* _importer = nullptr;
	wstring			_resourceDirectory;

	vector<FbxMeshInfo>					_meshes;
	vector<shared_ptr<FbxBoneInfo>>		_bones;
	vector<shared_ptr<FbxAnimClipInfo>>	_animClips;
	FbxArray<FbxString*>				_animNames;
};
