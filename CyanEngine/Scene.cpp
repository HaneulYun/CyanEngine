#include "pch.h"
#include "Scene.h"

Scene::Scene()
{
	rendererManager = RendererManager::Instance();
}

Scene::~Scene()
{
}

void Scene::Start()
{
	BuildObjects();

	//for (GameObject* gameObject : gameObjects)
	//	gameObject->Start();
	for (int i = 0; i < gameObjects.size(); ++i)
		gameObjects[i]->Start();
	rendererManager->UpdateManager();;
}

void Scene::Update()
{
	for (GameObject* gameObject : gameObjects)
		gameObject->Update();
	rendererManager->UpdateManager();
}

void Scene::Render()
{
	rendererManager->Render();
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

GameObject* Scene::CreateGameObject()
{
	GameObject* gameObject = new GameObject();
	gameObject->scene = this;

	return gameObject;
}

GameObject* Scene::AddGameObject()
{
	GameObject* gameObject = new GameObject();
	gameObject->scene = this;
	gameObjects.push_back(gameObject);

	return gameObject;
}

GameObject* Scene::AddGameObject(GameObject* gameObject)
{
	gameObject->scene = this;
	gameObjects.push_back(gameObject);

	return gameObject;
}

void Scene::RemoveGameObject(GameObject* gameObject)
{
	for (auto iter = gameObjects.begin(); iter != gameObjects.end(); ++iter)
		if (*iter == gameObject)
		{
			Renderer* renderer = gameObject->GetComponent<Renderer>();
			MeshFilter* meshFilter = gameObject->GetComponent<MeshFilter>();

			if (!meshFilter)
				meshFilter = gameObject->GetComponent<Terrain>();
			auto pair = std::pair<std::string, Mesh*>(typeid(renderer->material).name(), meshFilter->mesh);
			auto& list = rendererManager->instances[pair].second;
			for (auto mgrIter = list.begin(); mgrIter != list.end(); ++mgrIter)
				if (*mgrIter == gameObject)
				{
					list.erase(mgrIter);
					break;
				}

			delete (*iter);
			gameObjects.erase(iter);
			return;
			//iter = gameObjects.erase(iter);
		}
}
