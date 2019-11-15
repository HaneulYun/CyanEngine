#include "pch.h"
#include "Renderer.h"

Renderer::Renderer()
{
	rendererManager = RendererManager::Instance();
}

Renderer::~Renderer()
{
}

void Renderer::Start()
{
	Renderer* renderer = gameObject->GetComponent<Renderer>();
	MeshFilter* meshFilter = gameObject->GetComponent<MeshFilter>();
	if (!renderer)
		return;
	if (!meshFilter)
		return;
	
	auto pair = std::pair<Material*, Mesh*>(renderer->material, meshFilter->mesh);

	rendererManager->isRenewed = false;
	rendererManager->instances[pair].second.push_back(gameObject);
}

void Renderer::Update()
{
}

void Renderer::Render()
{
}

void Renderer::Destroy()
{
}