#pragma once

class AssetManager : public Singleton<AssetManager>
{
private:
	friend CyanFW;

	std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
	std::unordered_map<std::string, std::unique_ptr<Material>> materials;
	std::unordered_map<std::string, std::unique_ptr<GameObject>> prefabs;

public:
	std::unordered_map<std::string, std::unique_ptr<AnimationClip>> animationClips;

	std::vector<std::unique_ptr<AssetResource>> assetResource;

	void Update();

	void AddMesh(std::string name, std::unique_ptr<Mesh> mesh);
	void AddTexture(std::string name, std::wstring fileName);
	void AddMaterial(std::string name, Texture* diffuse = nullptr, Texture* noromal = nullptr,
		Vector4 albedo = { 1.0f, 1.0f, 1.0f, 1.0f }, Vector3 fresnel = { 0.01f, 0.01f, 0.01f },
		float roughness = 1.0f, Matrix4x4 matTransform = Matrix4x4::MatrixIdentity());
	void AddPrefab(std::string name, std::unique_ptr<GameObject> prefab);
	void AddFbxForMesh(std::string name, std::string fileNmae);
	void AddFbxForAnimation(std::string name, std::string fileNmae);

	Mesh* GetMesh(std::string name);
	Texture* GetTexture(std::string name);
	Material* GetMaterial(std::string name);
	GameObject* GetPrefab(std::string name);
};

#define ASSET AssetManager::Instance()->
#define MESH(name) GetMesh(name)
#define TEXTURE(name) GetTexture(name)
#define MATERIAL(name) GetMaterial(name)
#define PREFAB(name) GetPrefab(name)
