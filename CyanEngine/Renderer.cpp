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
	Renderer* renderer = static_cast<Renderer*>(gameObject->renderer); // GetComponent<Renderer>();
	MeshFilter* meshFilter = static_cast<MeshFilter*>(gameObject->meshFilter); // GetComponent<MeshFilter>();

	if (!renderer)
		return;
	if (!meshFilter)
		return;
	
	auto pair = std::pair<Shader*, Mesh*>(renderer->material->shader, meshFilter->mesh);

	const type_info* t = &typeid(pair);

	rendererManager->instances[pair].push_back(gameObject);
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