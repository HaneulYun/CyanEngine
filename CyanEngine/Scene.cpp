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

		M3DLoader::M3dMaterial mat{ "test" };
		modelData.skinnedMats.clear();
		modelData.skinnedMats.push_back(mat);

		modelData.LoadFbx("..\\CyanEngine\\Models\\modelTest2.fbx");
		animData.LoadFbx("..\\CyanEngine\\Models\\animTest2.fbx");

		std::unordered_map<std::string, AnimationClip> animations;
		animations["run"] = *animationClips["k"].get();
		mSkinnedInfo->Set(animData.parentIndexer, modelData.boneOffsets, animations);

		mSkinnedModelInst = new SkinnedModelInstance();
		mSkinnedModelInst->SkinnedInfo = mSkinnedInfo;
		mSkinnedModelInst->FinalTransforms.resize(mSkinnedInfo->BoneCount());
		//mSkinnedModelInst->ClipName = "Take1";
		mSkinnedModelInst->ClipName = "run";
		mSkinnedModelInst->TimePos = 0.0f;

		UINT matCBIndex = 4;
		UINT srvHeapIndex = 0;// mSkinnedSrvHeapStart;
		for (UINT i = 0; i < modelData.skinnedMats.size(); ++i)
		{
			auto mat = std::make_unique<Material>();
			mat->Name = modelData.skinnedMats[i].Name;
			mat->MatCBIndex = matCBIndex++;
			mat->DiffuseSrvHeapIndex = srvHeapIndex++;
			mat->NormalSrvHeapIndex = srvHeapIndex;// srvHeapIndex++;
			mat->DiffuseAlbedo = modelData.skinnedMats[i].DiffuseAlbedo;
			mat->FresnelR0 = modelData.skinnedMats[i].FresnelR0;
			mat->Roughness = modelData.skinnedMats[i].Roughness;

			materials[mat->Name] = std::move(mat);
		}
	}

	Graphics::Instance()->commandList->Close();
	ID3D12CommandList* cmdsLists[] = { Graphics::Instance()->commandList.Get() };
	Graphics::Instance()->commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);


	//for (int i = 0; i < modelData.skinnedMats.size(); ++i)
	//{
	//	std::string diffuseName = modelData.skinnedMats[i].DiffuseMapName;
	//	std::wstring diffuseFilename = L"..\\CyanEngine\\Textures\\" + AnsiToWString(diffuseName);
	//
	//	diffuseName = diffuseName.substr(0, diffuseName.find_last_of("."));
	//
	//	textureData.push_back({ diffuseName, diffuseFilename });
	//}
	//
	//std::vector<std::string> texName;
	//for (auto& d : textureData)
	//{
	//	if (textures.find(d.name) == std::end(textures))
	//	{
	//		auto texture = std::make_unique<Texture>();
	//		texture->Name = d.name;
	//		texture->Filename = d.fileName;
	//
	//		texName.push_back(d.name);
	//
	//		CreateDDSTextureFromFile12(
	//			Graphics::Instance()->device.Get(), Graphics::Instance()->commandList.Get(),
	//			texture->Filename.c_str(), texture->Resource, texture->UploadHeap);
	//		textures[texture->Name] = std::move(texture);
	//	}
	//}
	{
		UINT objCBIndex = allRItems.size();

		int count = 0;
		float interval = 2.5f;
		for (int x = -count; x <= count; ++x)
			for (int z = -count; z <= count; ++z)
				for (UINT i = 0; i < modelData.submeshes.size(); ++i)
				{
					std::string submeshName = "sm_" + std::to_string(i);

					auto ritem = CreateEmpty();
					ritem->GetComponent<Transform>()->Scale({ 0.02, 0.02, 0.02 });
					auto geo = ritem->AddComponent<MeshFilter>()->mesh = geometries[mSkinnedModelFilename].get();
					int n0 = ritem->GetComponent<MeshFilter>()->IndexCount = geo->DrawArgs[submeshName].IndexCount;
					int n1 = ritem->GetComponent<MeshFilter>()->StartIndexLocation = geo->DrawArgs[submeshName].StartIndexLocation;
					int n2 = ritem->GetComponent<MeshFilter>()->BaseVertexLocation = geo->DrawArgs[submeshName].BaseVertexLocation;

					ritem->TexTransform = MathHelper::Identity4x4();
					ritem->ObjCBIndex = objCBIndex++;
					//ritem->Mat = materials[modelData.skinnedMats[i].Name].get();
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

		//for (auto& d : texName)
		//{
		//	srvDesc.Format = textures[d]->Resource->GetDesc().Format;
		//	srvDesc.Texture2D.MipLevels = textures[d]->Resource->GetDesc().MipLevels;
		//	Graphics::Instance()->device->CreateShaderResourceView(textures[d]->Resource.Get(), &srvDesc, handle);
		//
		//	handle.Offset(1, Graphics::Instance()->srvDescriptorSize);
		//}
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