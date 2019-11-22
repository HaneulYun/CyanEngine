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

	for (int i = 0; i < gameObjects.size(); ++i)
		gameObjects[i]->Start();
}

void Scene::Update()
{
	for (GameObject* gameObject : gameObjects)
		gameObject->Update();
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

GameObject* Scene::CreateGameObject(GameObject* _gameObject)
{
	GameObject* gameObject = new GameObject(_gameObject);
	gameObject->scene = this;

	return gameObject;
}

GameObject* Scene::CreateEmpty()
{
	GameObject* gameObject = new GameObject();
	gameObject->scene = this;
	gameObjects.push_back(gameObject);

	return gameObject;
}

GameObject* Scene::Duplicate(GameObject* _gameObject)
{
	GameObject* gameObject = new GameObject(_gameObject);
	gameObject->scene = this;
	gameObjects.push_back(gameObject);

	return gameObject;
}

void Scene::Delete(GameObject* gameObject)
{
	for (auto iter = gameObjects.begin(); iter != gameObjects.end(); ++iter)
		if (*iter == gameObject)
		{
			delete (*iter);
			gameObjects.erase(iter);
			return;
		}
}
