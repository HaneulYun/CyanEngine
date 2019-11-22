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
		if(!(meshFilter = gameObject->GetComponent<Terrain>()))
			return;
	
	auto pair = std::pair<std::string, Mesh*>(typeid(renderer->material).name(), meshFilter->mesh);

	rendererManager->isRenewed = false;
	rendererManager->instances[pair].second.push_back(gameObject);
}

void Renderer::Update()
{
}