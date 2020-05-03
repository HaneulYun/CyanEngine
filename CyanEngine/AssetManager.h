#pragma once

class AssetManager : public Singleton<AssetManager>
{
public:
	std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;
	std::unordered_map<std::string, std::unique_ptr<Material>> materials;
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
	std::unordered_map<std::string, std::unique_ptr<AnimationClip>> animationClips;

	// Asset Managerment
	void AddTexture(std::string name, std::wstring fileName)
	{
		static int nTex{ 2 };
		if (AssetManager::Instance()->textures.find(name) != AssetManager::Instance()->textures.end())
			return;

		auto texture = std::make_unique<Texture>();
		texture->Name = name;
		texture->Filename = fileName;
		texture->Index = nTex++;
		AssetManager::Instance()->textures[texture->Name] = std::move(texture);
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
		AssetManager::Instance()->materials[material->Name] = std::move(material);
	}
	void AddFbxForMesh(std::string name, std::string fileNmae)
	{
		FbxModelData data;
		data.SetName(name);
		data.LoadFbx(fileNmae.c_str());
	}
	void AddFbxForAnimation(std::string name, std::string fileNmae)
	{
		FbxModelData data;
		data.SetName(name);
		data.LoadFbx(fileNmae.c_str());
	}
};

