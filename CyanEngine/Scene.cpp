#include "pch.h"
#include "Scene.h"

std::string mSkinnedModelFilename = "ApprenticeSK";

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Start()
{
	BuildObjects();

	Graphics::Instance()->commandList->Reset(Graphics::Instance()->commandAllocator.Get(), nullptr);

	FbxModelData modelData;
	FbxModelData animData;
	SkinnedModelInstance* mSkinnedModelInst;
	{
		SkinnedData* mSkinnedInfo = new SkinnedData();

		//M3DLoader::M3dMaterial mat{ "test" };
		//modelData.skinnedMats.clear();
		//modelData.skinnedMats.push_back(mat);

		modelData.LoadFbx("..\\CyanEngine\\Models\\modelTest2.fbx");
		animData.LoadFbx("..\\CyanEngine\\Models\\animTest2.fbx");

		std::unordered_map<std::string, AnimationClip> animations;
		animations["run"] = *animationClips["k"].get();
		mSkinnedInfo->Set(animData.parentIndexer, modelData.boneOffsets, animations);

		mSkinnedModelInst = new SkinnedModelInstance();
		mSkinnedModelInst->SkinnedInfo = mSkinnedInfo;
		mSkinnedModelInst->FinalTransforms.resize(mSkinnedInfo->BoneCount());
		mSkinnedModelInst->ClipName = "run";
		mSkinnedModelInst->TimePos = 0.0f;
	}
	for(int i = 0; i < 20; ++i)
	{
		auto material = std::make_unique<Material>();
		material->Name = "material_" + std::to_string(i);
		material->MatCBIndex = i;
		material->DiffuseSrvHeapIndex = 0;
		material->NormalSrvHeapIndex = 0;
		material->DiffuseAlbedo = RANDOM_COLOR;
		material->FresnelR0 = { 0.01f, 0.01f, 0.01f };
		material->Roughness = 0.0f;
		materials[material->Name] = std::move(material);
	}
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
		UINT objCBIndex = allRItems.size();

		int count = 0;
		float interval = 2.5f;
		for (int x = -count; x <= count; ++x)
			for (int z = -count; z <= count; ++z)
			{
				auto ritem = CreateEmpty();
				ritem->GetComponent<Transform>()->Scale({ 0.02, 0.02, 0.02 });
				ritem->GetComponent<Transform>()->Rotate({ 1, 0, 0 }, -90);
				ritem->GetComponent<Transform>()->position = { 2.0f * x, 0.0f, 2.0f * z };
				auto geo = ritem->AddComponent<SkinnedMeshRenderer>()->mesh = geometries[mSkinnedModelFilename].get();

				ritem->TexTransform = MathHelper::Identity4x4();
				ritem->ObjCBIndex = objCBIndex++;
				ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

				ritem->SkinnedCBIndex = 0;
				ritem->SkinnedModelInst = mSkinnedModelInst;

				renderItemLayer[(int)RenderLayer::SkinnedOpaque].push_back(ritem);
				allRItems.push_back(ritem);
			}
	}
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
			Graphics::Instance()->device.Get(), 1, (UINT)allRItems.size(), 1, (UINT)5));
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