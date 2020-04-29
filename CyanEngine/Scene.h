#pragma once

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
	std::map<Mesh*, RenderSets> renderObjectsLayer[(int)RenderLayer::Count];

	std::priority_queue<GameObject*> deletionQueue;
	std::vector<GameObject*> textObjects;

public:
	static Scene* scene;
	Camera* camera{ nullptr };

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

	GameObject* CreateEmpty();
	GameObject* Duplicate(GameObject*);
	GameObject* CreateEmptyPrefab();
	GameObject* DuplicatePrefab(GameObject*);

	GameObject* CreateUI();
	GameObject* CreateImage();
	GameObject* CreateImagePrefab();

	void PushDelete(GameObject*);
	void Delete(GameObject*);

	// Asset Managerment
	void AddTexture(UINT index, std::string name, std::wstring fileName)
	{
		auto texture = std::make_unique<Texture>();
		texture->Name = name;
		texture->Filename = fileName;
		texture->Index = index;
		textures[texture->Name] = std::move(texture);
	}
	void AddMaterial(UINT index, std::string name, int diffuse = -1, int noromal = -1,
		Vector4 albedo = { 1.0f, 1.0f, 1.0f, 1.0f }, Vector3 fresnel = { 0.01f, 0.01f, 0.01f },
		float roughness = 1.0f, Matrix4x4 matTransform = Matrix4x4::MatrixIdentity())
	{
		auto material = std::make_unique<Material>();
		material->Name = name;
		material->MatCBIndex = index;
		material->DiffuseSrvHeapIndex = diffuse;
		material->NormalSrvHeapIndex = noromal;
		material->DiffuseAlbedo = albedo;
		material->FresnelR0 = fresnel;
		material->Roughness = roughness;
		material->MatTransform = matTransform;
		materials[material->Name] = std::move(material);
	}
	void AddFbxForMesh(std::string name, std::string fileNmae)
	{
		FbxModelData data;
		data.SetScene(this);
		data.SetName(name);
		data.LoadFbx(fileNmae.c_str());
	}
	void AddFbxForAnimation(std::string name, std::string fileNmae)
	{
		FbxModelData data;
		data.SetScene(this);
		data.SetName(name);
		data.LoadFbx(fileNmae.c_str());
	}
};