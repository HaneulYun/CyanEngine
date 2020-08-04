#include "pch.h"
#include "AssetManager.h"

void AssetManager::Update()
{
	auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE();
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	for (auto& data : AssetManager::Instance()->textures)
	{
		auto texture = data.second.get();
		if (texture->Resource)
			continue;

		handle.InitOffsetted(Graphics::Instance()->GetSrv(20), texture->Index, Graphics::Instance()->srvDescriptorSize);
		CreateDDSTextureFromFile12(Graphics::Instance()->device.Get(), Graphics::Instance()->commandList.Get(), texture->Filename.c_str(), texture->Resource, texture->UploadHeap);
		srvDesc.Format = texture->Resource->GetDesc().Format;
		srvDesc.Texture2D.MipLevels = texture->Resource->GetDesc().MipLevels;

		Graphics::Instance()->device->CreateShaderResourceView(texture->Resource.Get(), &srvDesc, handle);
	}

	if (textures.find("skyTex") == textures.end())
	{
		auto skyTex = std::make_unique<Texture>();
		{
			skyTex->Name = "skyTex";
			skyTex->Filename = L"Textures\\Sky_Day_BlueSky_BC.dds";
			CreateDDSTextureFromFile12(Graphics::Instance()->device.Get(), Graphics::Instance()->commandList.Get(), skyTex->Filename.c_str(), skyTex->Resource, skyTex->UploadHeap);
			skyTex->Index = 0;
		}

		handle.InitOffsetted(Graphics::Instance()->GetSrv(10), skyTex->Index, Graphics::Instance()->srvDescriptorSize);
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = skyTex->Resource->GetDesc().MipLevels;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		srvDesc.Format = skyTex->Resource->GetDesc().Format;

		Graphics::Instance()->device->CreateShaderResourceView(skyTex->Resource.Get(), &srvDesc, handle);
		textures[skyTex->Name] = std::move(skyTex);
	}
}

void AssetManager::AddMesh(std::string name, std::unique_ptr<Mesh> mesh)
{
	if (AssetManager::Instance()->meshes.find(name) != AssetManager::Instance()->meshes.end())
		return;

	AssetManager::Instance()->meshes[name] = std::move(mesh);
}

void AssetManager::AddTexture(std::string name, std::wstring fileName)
{
	static int nTex{ 0 };
	if (AssetManager::Instance()->textures.find(name) != AssetManager::Instance()->textures.end())
		return;

	auto texture = std::make_unique<Texture>();
	texture->Name = name;
	texture->Filename = fileName;
	texture->Index = nTex++;
	AssetManager::Instance()->textures[texture->Name] = std::move(texture);
}

void AssetManager::AddMaterial(std::string name, Texture* diffuse, Texture* normal, Texture* emissive, Texture* mask,
	Vector4 albedo, Vector3 fresnel, float roughness, Matrix4x4 matTransform)
{	
	static int index{ 0 };
	if (AssetManager::Instance()->materials.find(name) != AssetManager::Instance()->materials.end())
		return;

	auto material = std::make_unique<Material>();
	material->Name = name;
	material->MatCBIndex = index++;
	material->DiffuseSrvHeapIndex = diffuse ? diffuse->Index : -1;
	material->NormalSrvHeapIndex = normal ? normal->Index : -1;
	material->EmissiveSrvHeapIndex = emissive ? emissive->Index : -1;
	material->MaskSrvHeapIndex = mask ? mask->Index : -1;
	material->DiffuseAlbedo = albedo;
	material->FresnelR0 = fresnel;
	material->Roughness = roughness;
	material->MatTransform = matTransform;
	AssetManager::Instance()->materials[material->Name] = std::move(material);
}

void AssetManager::AddPrefab(std::string name, std::unique_ptr<GameObject> prefab)
{
	if (AssetManager::Instance()->prefabs.find(name) != AssetManager::Instance()->prefabs.end())
		return;

	AssetManager::Instance()->prefabs[name] = std::move(prefab);
}

void AssetManager::AddAudioClip(std::string name, std::string path)
{
	if (AssetManager::Instance()->audioClips.find(name) != AssetManager::Instance()->audioClips.end())
		return;

	auto clip = std::make_unique<AudioClip>();
	clip->name = name;
	clip->path = path;
	AudioManager::Instance()->CreateSound(clip.get());

	AssetManager::Instance()->audioClips[name] = std::move(clip);
}

void AssetManager::AddFbxForMesh(std::string name, std::string fileNmae)
{
	if (auto iter = meshes.find(name); iter != meshes.end())
		return;
	FbxModelData data;
	data.SetName(name);
	data.LoadFbx(fileNmae.c_str());
}

void AssetManager::AddFbxForAnimation(std::string name, std::string fileNmae)
{
	if (auto iter = animationClips.find(name); iter != animationClips.end())
		return;
	FbxModelData data;
	data.SetName(name);
	data.LoadFbx(fileNmae.c_str());
}

Mesh* AssetManager::GetMesh(std::string name)
{
	if (auto iter = meshes.find(name); iter == meshes.end())
		return nullptr;
	return meshes[name].get();
}

Texture* AssetManager::GetTexture(std::string name)
{
	if (auto iter = textures.find(name); iter == textures.end())
		return nullptr;
	return textures[name].get();
}

Material* AssetManager::GetMaterial(std::string name)
{
	if (auto iter = materials.find(name); iter == materials.end())
		return nullptr;
	return materials[name].get();
}

AudioClip* AssetManager::GetAudioClip(std::string name)
{
	if (auto iter = audioClips.find(name); iter == audioClips.end())
		return nullptr;
	return audioClips[name].get();
}

GameObject* AssetManager::GetPrefab(std::string name)
{
	if (auto iter = prefabs.find(name); iter == prefabs.end())
		return nullptr;
	return prefabs[name].get();
}
