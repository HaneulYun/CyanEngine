#pragma once

struct ObjectsResource
{
	bool isDirty{ true };

	ObjectsResource() = default;
	ObjectsResource(const ObjectsResource&) = delete;
	ObjectsResource& operator=(const ObjectsResource&) = delete;
	~ObjectsResource() = default;

	std::unique_ptr<UploadBuffer<InstanceData>> InstanceBuffer{ nullptr };
	std::unique_ptr<UploadBuffer<SkinnnedData>> SkinnedBuffer{ nullptr };
	std::unique_ptr<UploadBuffer<MatIndexData>> MatIndexBuffer{ nullptr };
};

struct RenderObjects
{
	bool isDirty{ true };

	std::vector<GameObject*> gameObjects;

	std::vector<std::unique_ptr<ObjectsResource>> objectsResources;
};

struct Texture
{
	std::string Name;
	std::wstring Filename;

	UINT Index;

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

class Scene
{
private:
	bool isDirty{ true };

public:
	std::deque<GameObject*> gameObjects;
	std::map<Mesh*, RenderObjects> renderObjectsLayer[(int)RenderLayer::Count];

	std::priority_queue<GameObject*> deletionQueue;
	std::vector<GameObject*> textObjects;

public:
	static Scene* scene;

	std::vector<std::unique_ptr<FrameResource>> frameResources;

	std::unordered_map<std::string, std::unique_ptr<Mesh>> geometries;
	std::unordered_map<std::string, std::unique_ptr<Material>> materials;
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
	std::unordered_map<std::string, std::unique_ptr<AnimationClip>> animationClips;

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

	// Texture
	void AddTexture(UINT index, std::string name, std::wstring fileName)
	{
		auto texture = std::make_unique<Texture>();
		texture->Name = name;
		texture->Filename = fileName;
		texture->Index = index;
		textures[texture->Name] = std::move(texture);
	}
};