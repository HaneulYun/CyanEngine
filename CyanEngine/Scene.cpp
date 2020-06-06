#include "pch.h"
#include "Scene.h"

Scene* Scene::scene{ nullptr };

Scene::Scene()
{
	frameResourceManager.scene = this;
}

Scene::~Scene()
{
}

void Scene::Start()
{
	BuildObjects();

	AssetManager::Instance()->Update();

	for (int i = 0; i < NUM_FRAME_RESOURCES; ++i)
		frameResourceManager.frameResources.push_back(std::make_unique<FrameResource>(Graphics::Instance()->device.Get(), 2));
}

void Scene::Update()
{
	frameResourceManager.Update();

	while (!creationQueue.empty())
	{
		creationQueue.front()->Start();
		creationQueue.pop();
	}

	objectRenderManager.Update();

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
		for (auto iter = gameObject->collisionType.begin(); iter != gameObject->collisionType.end();)
		{
			switch (iter->second)
			{
			case CollisionType::eCollisionEnter:
				gameObject->OnCollisionEnter(iter->first); break;
			case CollisionType::eCollisionStay:
				gameObject->OnCollisionStay(iter->first); break;
			case CollisionType::eCollisionExit:
				gameObject->OnCollisionExit(iter->first); break;
			case CollisionType::eTriggerEnter:
				gameObject->OnTriggerEnter(iter->first); break;
			case CollisionType::eTriggerStay:
				gameObject->OnTriggerStay(iter->first); break;
			}
			if (iter->second == CollisionType::eTriggerExit)
			{
				gameObject->OnTriggerExit(iter->first);
				iter = gameObject->collisionType.erase(iter);
			}
			else
				++iter;
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
	gameObject->SetScene(this);
	gameObjects.push_back(gameObject);
	creationQueue.push(gameObject);
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

GameObject* Scene::CreateUIPrefab()
{
	GameObject* newGameObject = new GameObject(true);
	return newGameObject;
}

GameObject* Scene::CreateImage()
{
	GameObject* gameObject = CreateUI();

	auto mesh = gameObject->AddComponent<MeshFilter>()->mesh = ASSET MESH("Image");
	gameObject->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("none"));
	gameObject->AddComponent<Image>();
	gameObject->layer = (int)RenderLayer::UI;

	return gameObject;
}
GameObject* Scene::CreateImagePrefab()
{
	GameObject* gameObject = CreateUIPrefab();

	auto mesh = gameObject->AddComponent<MeshFilter>()->mesh = ASSET MESH("Image");
	gameObject->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("none"));
	gameObject->AddComponent<Image>();
	gameObject->layer = (int)RenderLayer::UI;

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
			gameObject->ReleaseRenderSet();

			delete (*iter);
			gameObjects.erase(iter);
			return;
		}
}
