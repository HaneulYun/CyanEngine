#pragma once

class AssetManager : public Singleton<AssetManager>
{
private:
public:
	std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;
	std::unordered_map<std::string, std::unique_ptr<Material>> materials;
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
	std::unordered_map<std::string, std::unique_ptr<AnimationClip>> animationClips;

	std::vector<std::unique_ptr<AssetResource>> assetResource;

	void Update();

	void AddMesh(std::string name, std::unique_ptr<Mesh> mesh);
	void AddTexture(std::string name, std::wstring fileName);
	void AddMaterial(std::string name, int diffuse = -1, int noromal = -1,
		Vector4 albedo = { 1.0f, 1.0f, 1.0f, 1.0f }, Vector3 fresnel = { 0.01f, 0.01f, 0.01f },
		float roughness = 1.0f, Matrix4x4 matTransform = Matrix4x4::MatrixIdentity());
	void AddFbxForMesh(std::string name, std::string fileNmae);
	void AddFbxForAnimation(std::string name, std::string fileNmae);
};
