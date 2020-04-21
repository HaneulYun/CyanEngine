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
	modelData.LoadFbx("..\\CyanEngine\\Models\\modelTest.fbx");
	FbxModelData animData0;
	animData0.LoadFbx("..\\CyanEngine\\Models\\BowStance\\Idle_BowAnim.fbx",			"Attack01_BowAnim");
	FbxModelData animData1;
	animData1.LoadFbx("..\\CyanEngine\\Models\\BowStance\\DieRecover_BowAnim.fbx",	"Attack01Maintain_BowAnim");
	FbxModelData animData2;
	animData2.LoadFbx("..\\CyanEngine\\Models\\BowStance\\JumpEnd_BowAnim.fbx",	"Attack01RepeatFire_BowAnim");
	FbxModelData animData3;
	animData3.LoadFbx("..\\CyanEngine\\Models\\BowStance\\Sprint_BowAnim.fbx",		"Attack01Start_BowAnim");
	FbxModelData animData4;
	animData4.LoadFbx("..\\CyanEngine\\Models\\BowStance\\WalkBack_BowAnim.fbx",	"Attack02Maintain_BowAnim");
	FbxModelData animData5;
	animData5.LoadFbx("..\\CyanEngine\\Models\\BowStance\\RollBack_Bow.fbx",	"Attack02RepeatFire_BowAnim");
	FbxModelData animData6;
	animData6.LoadFbx("..\\CyanEngine\\Models\\BowStance\\GetHit_BowAnim.fbx",		"Attack02Start_BowAnim");
	FbxModelData animData7;
	animData7.LoadFbx("..\\CyanEngine\\Models\\BowStance\\EquipmentChange_BowAnim.fbx",		"DashBackward_BowAnim");
	FbxModelData animData8;
	animData8.LoadFbx("..\\CyanEngine\\Models\\BowStance\\DashForward_BowAnim.fbx",			"DashForward_BowAnim");


	FbxModelData animData10;
	animData10.LoadFbx("..\\CyanEngine\\Models\\BowStance\\Walk_BowAnim.fbx",		"Walk_BowAnim");
	FbxModelData animData11;
	animData11.LoadFbx("..\\CyanEngine\\Models\\BowStance\\WalkBack_BowAnim.fbx",	"WalkBack_BowAnim");
	FbxModelData animData12;
	animData12.LoadFbx("..\\CyanEngine\\Models\\BowStance\\WalkRight_BowAnim.fbx",	"WalkRight_BowAnim");
	FbxModelData animData13;
	animData13.LoadFbx("..\\CyanEngine\\Models\\BowStance\\WalkLeft_BowAnim.fbx",	"WalkLeft_BowAnim");
	FbxModelData animData14;
	animData14.LoadFbx("..\\CyanEngine\\Models\\BowStance\\Idle_BowAnim.fbx",		"Idle_BowAnim");
	
	BuildObjects();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(Graphics::Instance()->srvHeap->GetCPUDescriptorHandleForHeapStart());
	for (auto& d : textures)
	{
		auto texture = d.second.get();
		CreateDDSTextureFromFile12(Graphics::Instance()->device.Get(), Graphics::Instance()->commandList.Get(),
			texture->Filename.c_str(), texture->Resource, texture->UploadHeap);
	
		srvDesc.Format = texture->Resource->GetDesc().Format;
		srvDesc.Texture2D.MipLevels = texture->Resource->GetDesc().MipLevels;

		handle.InitOffsetted(Graphics::Instance()->srvHeap->GetCPUDescriptorHandleForHeapStart(),
			texture->Index + 1, Graphics::Instance()->srvDescriptorSize);
		Graphics::Instance()->device->CreateShaderResourceView(texture->Resource.Get(), &srvDesc, handle);
	}

	Graphics::Instance()->commandList->Close();
	ID3D12CommandList* cmdsLists[] = { Graphics::Instance()->commandList.Get() };
	Graphics::Instance()->commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	for (int i = 0; i < NUM_FRAME_RESOURCES; ++i)
		frameResources.push_back(std::make_unique<FrameResource>(Graphics::Instance()->device.Get(), 1, (UINT)5));
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