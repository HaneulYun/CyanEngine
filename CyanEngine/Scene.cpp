#include "pch.h"
#include "Scene.h"

Scene* Scene::scene{ nullptr };

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Start()
{
	Graphics::Instance()->commandList->Reset(Graphics::Instance()->commandAllocator.Get(), nullptr);
	
	auto skyTex = std::make_unique<Texture>();
	{
		skyTex->Name = "skyTex";
		skyTex->Filename = L"Textures\\grasscube1024.dds";
		CreateDDSTextureFromFile12(Graphics::Instance()->device.Get(), Graphics::Instance()->commandList.Get(),
			skyTex->Filename.c_str(), skyTex->Resource, skyTex->UploadHeap);
		skyTex->Index = 0;
	}
	BuildObjects();

	auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(Graphics::Instance()->srvHeap->GetCPUDescriptorHandleForHeapStart());
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	for (auto& data : textures)
	{
		auto texture = data.second.get();
		texture->Index += 2;
		handle.InitOffsetted(Graphics::Instance()->srvHeap->GetCPUDescriptorHandleForHeapStart(), texture->Index, Graphics::Instance()->srvDescriptorSize);
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
	textures[skyTex->Name] = std::move(skyTex);

	Graphics::Instance()->commandList->Close();
	ID3D12CommandList* cmdsLists[] = { Graphics::Instance()->commandList.Get() };
	Graphics::Instance()->commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	for (int i = 0; i < NUM_FRAME_RESOURCES; ++i)
		frameResources.push_back(std::make_unique<FrameResource>(Graphics::Instance()->device.Get(), 1, (UINT)10));
}

void Scene::Update()
{
	// fixed update
	Collider *lhs_collider, *rhs_collider;
	for (auto lhs_iter = gameObjects.begin(); lhs_iter != gameObjects.end(); ++lhs_iter)
		if (lhs_collider = (*lhs_iter)->GetComponent<Collider>())
			for (auto rhs_iter = gameObjects.begin(); rhs_iter != gameObjects.end(); ++rhs_iter)
				if(lhs_iter != rhs_iter)
					if (rhs_collider = (*rhs_iter)->GetComponent<Collider>())
					{
						auto iter = (*lhs_iter)->collisionType.find(*rhs_iter);
	
						if (lhs_collider->Compare(rhs_collider))
						{
							if (iter == (*lhs_iter)->collisionType.end())
								((*lhs_iter)->collisionType)[*rhs_iter] = CollisionType::eTriggerEnter;
							else if (iter->second == CollisionType::eTriggerEnter)
								iter->second = CollisionType::eTriggerStay;
						}
						else if (iter != (*lhs_iter)->collisionType.end())
							iter->second = CollisionType::eTriggerExit;
					}

	for (GameObject* gameObject : gameObjects)
	{
		for (auto d : gameObject->collisionType)
		{
			switch (d.second)
			{
			case CollisionType::eCollisionEnter:
				gameObject->OnCollisionEnter(d.first); break;
			case CollisionType::eCollisionStay:
				gameObject->OnCollisionStay(d.first); break;
			case CollisionType::eCollisionExit:
				gameObject->OnCollisionExit(d.first); break;
			case CollisionType::eTriggerEnter:
				gameObject->OnTriggerEnter(d.first); break;
			case CollisionType::eTriggerStay:
				gameObject->OnTriggerStay(d.first); break;
			case CollisionType::eTriggerExit:
				gameObject->OnTriggerExit(d.first); break;
			}
		}
	}

	// input process
	for (GameObject* gameObject : gameObjects)
		if (auto button = gameObject->GetComponent<Button>(); button)
			button->OnClick();

	// update
	for (GameObject* gameObject : gameObjects)
		gameObject->Update();

	while (!deletionQueue.empty())
	{
		GameObject* gameObject = deletionQueue.top();
		Delete(gameObject);
		deletionQueue.pop();
	}
}

void Scene::BuildObjects()
{
}

void Scene::ReleaseObjects()
{
	for (GameObject* object : gameObjects)
		delete object;
	gameObjects.clear();
}


GameObject* Scene::AddGameObject(GameObject* gameObject)
{
	gameObject->scene = this;
	gameObjects.push_back(gameObject);
	return gameObject;
}

GameObject* Scene::CreateEmpty()
{
	GameObject* newGameObject = new GameObject(false);
	AddGameObject(newGameObject);
	return newGameObject;
}

GameObject* Scene::Duplicate(GameObject* gameObject)
{
	GameObject* newGameObject = new GameObject(gameObject);
	AddGameObject(newGameObject);
	return newGameObject;
}

GameObject* Scene::CreateEmptyPrefab()
{
	return new GameObject(false);
}

GameObject* Scene::DuplicatePrefab(GameObject* gameObject)
{
	return new GameObject(gameObject);
}

GameObject* Scene::CreateUI()
{
	GameObject* newGameObject = new GameObject(true);
	AddGameObject(newGameObject);
	return newGameObject;
}

GameObject* Scene::CreateImage()
{
	GameObject* gameObject = CreateUI();

	auto mesh = gameObject->AddComponent<MeshFilter>()->mesh = geometries["Image"].get();;
	gameObject->AddComponent<Renderer>()->materials.push_back(0);
	gameObject->AddComponent<Image>();
	renderObjectsLayer[(int)RenderLayer::UI][mesh].gameObjects.push_back(gameObject);

	return gameObject;
}
GameObject* Scene::CreateImagePrefab()
{
	GameObject* gameObject = new GameObject(true);

	auto mesh = gameObject->AddComponent<MeshFilter>()->mesh = geometries["Image"].get();;
	gameObject->AddComponent<Renderer>()->materials.push_back(0);
	gameObject->AddComponent<Image>();
	renderObjectsLayer[(int)RenderLayer::UI][mesh].gameObjects.push_back(gameObject);

	return gameObject;
}

void Scene::PushDelete(GameObject* gameObject)
{
	deletionQueue.push(gameObject);
}

void Scene::Delete(GameObject* gameObject)
{
	for (auto iter = gameObjects.begin(); iter != gameObjects.end(); ++iter)
		if ((*iter)->collisionType.find(gameObject) != (*iter)->collisionType.end())
			(*iter)->collisionType.erase(gameObject);
	for (auto iter = gameObjects.begin(); iter != gameObjects.end(); ++iter)
		if (*iter == gameObject)
		{
			delete (*iter);
			gameObjects.erase(iter);
			return;
		}
}