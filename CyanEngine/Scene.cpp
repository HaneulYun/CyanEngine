#include "pch.h"
#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Start()
{
	Graphics::Instance()->commandList->Reset(Graphics::Instance()->commandAllocator.Get(), nullptr);

	FbxModelData modelData;
	FbxModelData animData;
	modelData.LoadFbx("..\\CyanEngine\\Models\\modelTest.fbx");
	animData.LoadFbx("..\\CyanEngine\\Models\\animTest.fbx");

	mSkinnedInfo = new AnimatorController();
	{
		std::unordered_map<std::string, AnimationClip> animations;
		animations["run"] = *animationClips["k"].get();
		mSkinnedInfo->Set(modelData.parentIndexer, modelData.boneOffsets, animations);
	}
	
	BuildObjects();

	{
		auto texture = std::make_unique<Texture>();
		texture->Name = "texture";
		texture->Filename = L"..\\CyanEngine\\Textures\\none.dds";
		CreateDDSTextureFromFile12(Graphics::Instance()->device.Get(), Graphics::Instance()->commandList.Get(), texture->Filename.c_str(), texture->Resource, texture->UploadHeap);
		textures[texture->Name] = std::move(texture);
	}

	Graphics::Instance()->commandList->Close();
	ID3D12CommandList* cmdsLists[] = { Graphics::Instance()->commandList.Get() };
	Graphics::Instance()->commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	{
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(Graphics::Instance()->srvHeap->GetCPUDescriptorHandleForHeapStart());

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		for (auto& d : textures)
		{
			srvDesc.Format = d.second->Resource->GetDesc().Format;
			srvDesc.Texture2D.MipLevels = d.second->Resource->GetDesc().MipLevels;
			Graphics::Instance()->device->CreateShaderResourceView(d.second->Resource.Get(), &srvDesc, handle);
		
			handle.Offset(1, Graphics::Instance()->srvDescriptorSize);
		}
	}

	for (int i = 0; i < NUM_FRAME_RESOURCES; ++i)
		frameResources.push_back(std::make_unique<FrameResource>(
			Graphics::Instance()->device.Get(), 1,
			(UINT)gameObjects.size(),
			(UINT)gameObjects.size() * mSkinnedInfo->BoneCount(),
			(UINT)5,
			(UINT)gameObjects.size() * 20));
}

void Scene::Update()
{
	if (isDirty)
	{
		isDirty = false;
		Start();
	}

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

GameObject* Scene::CreateEmpty(bool addition)
{
	return new GameObject(addition);
}

GameObject* Scene::Duplicate(GameObject* gameObject)
{
	return new GameObject(gameObject);
}

GameObject* Scene::CreateEmptyPrefab()
{
	return new GameObject(false);
}

GameObject* Scene::DuplicatePrefab(GameObject* gameObject)
{
	return new GameObject(gameObject, false);
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