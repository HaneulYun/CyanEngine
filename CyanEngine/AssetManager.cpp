#include "pch.h"
#include "AssetManager.h"

void AssetManager::Update()
{
	Graphics::Instance()->commandList->Reset(Graphics::Instance()->commandAllocator.Get(), nullptr);

	auto skyTex = std::make_unique<Texture>();
	{
		skyTex->Name = "skyTex";
		skyTex->Filename = L"Textures\\grasscube1024.dds";
		CreateDDSTextureFromFile12(Graphics::Instance()->device.Get(), Graphics::Instance()->commandList.Get(), skyTex->Filename.c_str(), skyTex->Resource, skyTex->UploadHeap);
		skyTex->Index = 0;
	}

	auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(Graphics::Instance()->srvHeap->GetCPUDescriptorHandleForHeapStart());
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	for (auto& data : AssetManager::Instance()->textures)
	{
		auto texture = data.second.get();
		handle.InitOffsetted(Graphics::Instance()->srvHeap->GetCPUDescriptorHandleForHeapStart(), texture->Index, Graphics::Instance()->srvDescriptorSize);
		if (!texture->Resource)
			CreateDDSTextureFromFile12(Graphics::Instance()->device.Get(), Graphics::Instance()->commandList.Get(), texture->Filename.c_str(), texture->Resource, texture->UploadHeap);
		srvDesc.Format = texture->Resource->GetDesc().Format;
		srvDesc.Texture2D.MipLevels = texture->Resource->GetDesc().MipLevels;

		Graphics::Instance()->device->CreateShaderResourceView(texture->Resource.Get(), &srvDesc, handle);
	}
	handle.InitOffsetted(Graphics::Instance()->srvHeap->GetCPUDescriptorHandleForHeapStart(), 0, Graphics::Instance()->srvDescriptorSize);

	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = skyTex->Resource->GetDesc().MipLevels;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	srvDesc.Format = skyTex->Resource->GetDesc().Format;
	Graphics::Instance()->device->CreateShaderResourceView(skyTex->Resource.Get(), &srvDesc, handle);
	AssetManager::Instance()->textures[skyTex->Name] = std::move(skyTex);

	Graphics::Instance()->commandList->Close();
	ID3D12CommandList* cmdsLists[] = { Graphics::Instance()->commandList.Get() };
	Graphics::Instance()->commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

void AssetManager::AddMesh(std::string name, std::unique_ptr<Mesh> mesh)
{
	if (AssetManager::Instance()->meshes.find(name) != AssetManager::Instance()->meshes.end())
		return;

	AssetManager::Instance()->meshes[name] = std::move(mesh);
}

void AssetManager::AddTexture(std::string name, std::wstring fileName)
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

void AssetManager::AddMaterial(std::string name, int diffuse, int noromal,
	Vector4 albedo, Vector3 fresnel, float roughness, Matrix4x4 matTransform)
{	
	static int index{ 0 };
	if (AssetManager::Instance()->materials.find(name) != AssetManager::Instance()->materials.end())
		return;

	auto material = std::make_unique<Material>();
	material->Name = name;
	material->MatCBIndex = index++;
	material->DiffuseSrvHeapIndex = diffuse;
	material->NormalSrvHeapIndex = noromal;
	material->DiffuseAlbedo = albedo;
	material->FresnelR0 = fresnel;
	material->Roughness = roughness;
	material->MatTransform = matTransform;
	AssetManager::Instance()->materials[material->Name] = std::move(material);
}

void AssetManager::AddFbxForMesh(std::string name, std::string fileNmae)
{
	FbxModelData data;
	data.SetName(name);
	data.LoadFbx(fileNmae.c_str());
}

void AssetManager::AddFbxForAnimation(std::string name, std::string fileNmae)
{
	FbxModelData data;
	data.SetName(name);
	data.LoadFbx(fileNmae.c_str());
}
