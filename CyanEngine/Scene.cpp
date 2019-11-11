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

	for (GameObject* gameObject : gameObjects)
		gameObject->Start();
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