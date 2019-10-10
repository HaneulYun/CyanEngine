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
	//Renderer* renderer = gameObject->GetComponent<Renderer>();
	
	rendererManager->instances;;
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