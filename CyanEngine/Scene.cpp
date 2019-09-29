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
}

void Scene::Update()
{
	renderer->Update();
}

void Scene::Render()
{
	renderer->Render();
}

void Scene::OnDestroy()
{
	renderer->OnDestroy();
}
