#pragma once

struct SkinnedModelInstance
{
	SkinnedData* SkinnedInfo = nullptr;
	std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
	std::string ClipName;
	float TimePos = 0.0f;

	void UpdateSkinnedAnimation(float dt)
	{
		TimePos += dt;

		float k = SkinnedInfo->GetClipEndTime(ClipName);

		if (TimePos > SkinnedInfo->GetClipEndTime(ClipName))
			TimePos = 0.0f;

		SkinnedInfo->GetFinalTransforms(ClipName, TimePos, FinalTransforms);
	}
};

struct RenderItem
{
	RenderItem() = default;
	RenderItem(const RenderItem& rhs) = delete;

	XMFLOAT4X4 World = MathHelper::Identity4x4();
	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	int NumFramesDirty{ NUM_FRAME_RESOURCES };
	UINT ObjCBIndex{ UINT(-1) };

	Material* Mat{ nullptr };
	MeshGeometry* Geo{ nullptr };

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	UINT IndexCount{ 0 };
	UINT StartIndexLocation{ 0 };
	int BaseVertexLocation{ 0 };

	UINT SkinnedCBIndex = -1;
	SkinnedModelInstance* SkinnedModelInst = nullptr;
};

enum class RenderLayer : int
{
	Opaque = 0,
	SkinnedOpaque,
	Debug,
	Sky,
	Count
};

struct TextureData
{
	std::string name;
	std::wstring fileName;
};

struct FbxModelData
{
	struct BoneWeightData
	{
		unsigned int boneIndex;
		double weight;
	};

	std::map<int, FbxNode*> nodes;
	std::map<int, std::vector<BoneWeightData>> boneWeightData;
	std::vector<int> parentIndexer;
	std::map<std::string, int> skeletonIndexer;
	std::vector<M3DLoader::Subset> skinnedSubsets;
	std::vector<M3DLoader::M3dMaterial> skinnedMats;
	std::vector<XMFLOAT4X4> boneOffsets;

	// Assets Management
	void LoadFbx(const char* path);
	void LoadFbxHierarchy(FbxNode* node);
	void LoadFbxHierarchyRecursive(FbxNode* node, int parentIndex = -1);
	void LoadFbxMesh(FbxNode* node);
};

class Scene
{
private:
	bool isDirty{ true };

protected:
	std::deque<GameObject*> gameObjects;
	std::priority_queue<GameObject*> deletionQueue;

public:
	static Scene* scene;

	std::vector<std::unique_ptr<FrameResource>> frameResources;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> geometries;
	std::unordered_map<std::string, std::unique_ptr<Material>> materials;
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
	std::unordered_map<std::string, std::unique_ptr<AnimationClip>> animationClips;
	std::vector<TextureData> textureData;

	std::vector<std::unique_ptr<RenderItem>> allRItems;
	std::vector<RenderItem*> renderItemLayer[(int)RenderLayer::Count];

public:
	Scene();
	virtual ~Scene();

	virtual void Start();
	virtual void Update();

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	// GameObject Management
	GameObject* AddGameObject(GameObject*);
	GameObject* CreateEmpty(bool = true);
	GameObject* Duplicate(GameObject*);
	GameObject* CreateEmptyPrefab();
	GameObject* DuplicatePrefab(GameObject*);

	void PushDelete(GameObject*);
	void Delete(GameObject*);
};