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
	BuildObjects();

	Graphics::Instance()->commandList->Reset(Graphics::Instance()->commandAllocator.Get(), nullptr);

	std::string mSkinnedModelFilename = "Models\\soldier.m3d";

	std::vector<M3DLoader::Subset> mSkinnedSubsets;
	std::vector<M3DLoader::M3dMaterial> mSkinnedMats;
	SkinnedModelInstance* mSkinnedModelInst;
	{
		SkinnedData* mSkinnedInfo = new SkinnedData();

		std::vector<M3DLoader::SkinnedVertex> vertices;
		std::vector<std::uint16_t> indices;

		M3DLoader m3dLoader;
		m3dLoader.LoadM3d("..\\CyanEngine\\Models\\soldier.m3d", vertices, indices,
			mSkinnedSubsets, mSkinnedMats, *mSkinnedInfo);

		//{
		//	FbxManager* manager = FbxManager::Create();
		//
		//	FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
		//	manager->SetIOSettings(ios);
		//
		//	FbxScene* scene = FbxScene::Create(manager, "Scene");
		//
		//	FbxImporter* importer = FbxImporter::Create(manager, "");
		//	importer->Initialize("..\\CyanEngine\\Models\\modelTest.fbx");
		//	importer->Import(scene);
		//
		//	FbxScene* scene2 = FbxScene::Create(manager, "Scene2");
		//
		//	FbxImporter* importer2 = FbxImporter::Create(manager, "");
		//	importer2->Initialize("..\\CyanEngine\\Models\\animTest.fbx");
		//	importer2->Import(scene2);
		//
		//	{
		//		FbxGeometryConverter geometryConverter(manager);
		//		geometryConverter.Triangulate(scene, true);
		//
		//		std::vector<XMFLOAT4X4> boneOffsets;
		//		std::vector<int> boneIndexToParentIndex;
		//		std::unordered_map<std::string, AnimationClip> animations;
		//		AnimationClip clip;
		//
		//		vertices.clear();
		//		indices.clear();
		//		for (auto& offset : boneOffsets)
		//			offset = MathHelper::Identity4x4();
		//
		//		LoadModel(scene->GetRootNode(), vertices, indices, boneOffsets, boneIndexToParentIndex);
		//		boneIndexToParentIndex.clear();
		//		clip.BoneAnimations.resize(boneOffsets.size());
		//		LoadAnimation(scene2->GetRootNode(), clip, boneIndexToParentIndex);
		//
		//		animations["run"] = clip;
		//
		//		delete mSkinnedInfo;
		//		mSkinnedInfo = new SkinnedData();
		//		mSkinnedInfo->Set(boneIndexToParentIndex, boneOffsets, animations);
		//	}
		//
		//	if (manager)
		//		manager->Destroy();
		//}
		mSkinnedSubsets.resize(5);

		//for (UINT i = 0; i < 5; ++i)
		//{
		//	mSkinnedSubsets[i].Id = 0;
		//	mSkinnedSubsets[i].VertexStart = 0;
		//	mSkinnedSubsets[i].VertexCount = vertices.size();
		//	mSkinnedSubsets[i].FaceStart = 0;
		//	mSkinnedSubsets[i].FaceCount = indices.size();
		//}

		mSkinnedModelInst = new SkinnedModelInstance();
		mSkinnedModelInst->SkinnedInfo = mSkinnedInfo;
		mSkinnedModelInst->FinalTransforms.resize(mSkinnedInfo->BoneCount());
		mSkinnedModelInst->ClipName = "Take1";
		// mSkinnedModelInst->ClipName = "run";
		mSkinnedModelInst->TimePos = 0.0f;

		const UINT vbByteSize = (UINT)vertices.size() * sizeof(FrameResource::SkinnedVertex);
		const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

		auto geo = std::make_unique<MeshGeometry>();
		geo->Name = mSkinnedModelFilename;

		ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
		CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

		ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
		CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(
			Graphics::Instance()->device.Get(), Graphics::Instance()->commandList.Get(),
			vertices.data(), vbByteSize, geo->VertexBufferUploader);
		geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(
			Graphics::Instance()->device.Get(), Graphics::Instance()->commandList.Get(),
			indices.data(), ibByteSize, geo->IndexBufferUploader);

		geo->VertexByteStride = sizeof(FrameResource::SkinnedVertex);
		geo->VertexBufferByteSize = vbByteSize;
		geo->IndexFormat = DXGI_FORMAT_R16_UINT;
		geo->IndexBufferByteSize = ibByteSize;

		for (UINT i = 0; i < (UINT)mSkinnedSubsets.size(); ++i)
		{
			SubmeshGeometry submesh;
			std::string name = "sm_" + std::to_string(i);

			submesh.IndexCount = (UINT)mSkinnedSubsets[i].FaceCount * 3;
			submesh.StartIndexLocation = mSkinnedSubsets[i].FaceStart * 3;
			submesh.BaseVertexLocation = 0;

			geo->DrawArgs[name] = submesh;
		}

		Scene::scene->geometries[geo->Name] = std::move(geo);


		UINT matCBIndex = 4;
		UINT srvHeapIndex = 0;// mSkinnedSrvHeapStart;
		for (UINT i = 0; i < mSkinnedMats.size(); ++i)
		{
			auto mat = std::make_unique<Material>();
			mat->Name = mSkinnedMats[i].Name;
			mat->MatCBIndex = matCBIndex++;
			mat->DiffuseSrvHeapIndex = srvHeapIndex++;
			mat->NormalSrvHeapIndex = srvHeapIndex;// srvHeapIndex++;
			mat->DiffuseAlbedo = mSkinnedMats[i].DiffuseAlbedo;
			mat->FresnelR0 = mSkinnedMats[i].FresnelR0;
			mat->Roughness = mSkinnedMats[i].Roughness;

			Scene::scene->materials[mat->Name] = std::move(mat);
		}
	}
	for (int i = 0; i < mSkinnedMats.size(); ++i)
	{
		std::string diffuseName = mSkinnedMats[i].DiffuseMapName;
		std::wstring diffuseFilename = L"..\\CyanEngine\\Textures\\" + AnsiToWString(diffuseName);

		diffuseName = diffuseName.substr(0, diffuseName.find_last_of("."));

		textureData.push_back({ diffuseName, diffuseFilename });
	}

	std::vector<std::string> texName;
	for (auto& d : textureData)
	{
		if (Scene::scene->textures.find(d.name) == std::end(Scene::scene->textures))
		{
			auto texture = std::make_unique<Texture>();
			texture->Name = d.name;
			texture->Filename = d.fileName;

			texName.push_back(d.name);

			CreateDDSTextureFromFile12(
				Graphics::Instance()->device.Get(), Graphics::Instance()->commandList.Get(),
				texture->Filename.c_str(), texture->Resource, texture->UploadHeap);
			Scene::scene->textures[texture->Name] = std::move(texture);
		}
	}
	{
		UINT objCBIndex = Scene::scene->allRItems.size();

		int count = 0;
		float interval = 2.5f;
		for (int x = -count; x <= count; ++x)
			for (int z = -count; z <= count; ++z)
				for (UINT i = 0; i < mSkinnedMats.size(); ++i)
				{
					std::string submeshName = "sm_" + std::to_string(i);

					auto ritem = std::make_unique<RenderItem>();

					//XMMATRIX modelScale = XMMatrixScaling(0.005, 0.005, 0.005);
					//XMMATRIX modelRot = XMMatrixRotationX(-PI * 0.5);
					XMMATRIX modelScale = XMMatrixScaling(0.05, 0.05, -0.05);
					XMMATRIX modelRot = XMMatrixRotationX(0);
					XMMATRIX modelOffset = XMMatrixTranslation(x * 3, 0, z * 3);
					XMStoreFloat4x4(&ritem->World, modelScale * modelRot * modelOffset);

					ritem->TexTransform = MathHelper::Identity4x4();
					ritem->ObjCBIndex = objCBIndex++;
					ritem->Mat = Scene::scene->materials[mSkinnedMats[i].Name].get();
					ritem->Geo = Scene::scene->geometries[mSkinnedModelFilename].get();
					ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
					ritem->IndexCount = ritem->Geo->DrawArgs[submeshName].IndexCount;
					ritem->StartIndexLocation = ritem->Geo->DrawArgs[submeshName].StartIndexLocation;
					ritem->BaseVertexLocation = ritem->Geo->DrawArgs[submeshName].BaseVertexLocation;

					ritem->SkinnedCBIndex = 0;
					ritem->SkinnedModelInst = mSkinnedModelInst;

					Scene::scene->renderItemLayer[(int)RenderLayer::SkinnedOpaque].push_back(ritem.get());
					Scene::scene->allRItems.push_back(std::move(ritem));
				}
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

		for (auto& d : texName)
		{
			srvDesc.Format = Scene::scene->textures[d]->Resource->GetDesc().Format;
			srvDesc.Texture2D.MipLevels = Scene::scene->textures[d]->Resource->GetDesc().MipLevels;
			Graphics::Instance()->device->CreateShaderResourceView(Scene::scene->textures[d]->Resource.Get(), &srvDesc, handle);

			handle.Offset(1, Graphics::Instance()->srvDescriptorSize);
		}
	}

	for (int i = 0; i < NUM_FRAME_RESOURCES; ++i)
		frameResources.push_back(std::make_unique<FrameResource>(
			Graphics::Instance()->device.Get(), 1, (UINT)Scene::scene->allRItems.size(), 1, (UINT)5));
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