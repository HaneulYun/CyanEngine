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
	rendererManager->Start();;
}

void Scene::Update()
{
	for (GameObject* gameObject : gameObjects)
		gameObject->Update();
	rendererManager->Update();
}

void Scene::Render()
{
	rendererManager->Render();
}

void Scene::Destroy()
{
	ReleaseObjects();
	rendererManager->Destroy();
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