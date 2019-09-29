#include "pch.h"
#include "Scene.h"

Scene::Scene()
{
	renderer = Renderer::Instance();
}

Scene::~Scene()
{
}

void Scene::OnStart()
{
	renderer->OnStart();
	BuildObjects();
}

void Scene::Update()
{
	AnimateObjects();
	renderer->Update();
}

void Scene::Render()
{
	renderer->Render();
}

void Scene::OnDestroy()
{
	ReleaseObjects();
	renderer->OnDestroy();
}

void Scene::BuildObjects()
{
}

void Scene::ReleaseObjects()
{
}

void Scene::AnimateObjects()
{
}