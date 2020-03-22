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
	
	//auto pair = std::pair<std::string, Mesh*>(typeid(renderer->material).name(), meshFilter->mesh);

	rendererManager->isRenewed = false;
	//rendererManager->instances[pair].second.push_back(gameObject);

	{
		auto item = std::make_unique<RenderItem>();

		//Vector3 position = gameObject->GetComponent<Transform>()->position;
		//Vector3 scale = gameObject->GetComponent<Transform>()->localScale;

		item->World = gameObject->transform->localToWorldMatrix;
		XMStoreFloat4x4(&item->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
		item->ObjCBIndex = rendererManager->allRItems.size();
		item->Mat = material;
		item->Geo = meshFilter->mesh;
		item->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		item->IndexCount = item->Geo->DrawArgs["submesh"].IndexCount;
		item->StartIndexLocation = item->Geo->DrawArgs["submesh"].StartIndexLocation;
		item->BaseVertexLocation = item->Geo->DrawArgs["submesh"].BaseVertexLocation;

		rendererManager->renderItemLayer[(int)RenderLayer::Opaque].push_back(item.get());
		rendererManager->allRItems.push_back(std::move(item));
	}
}

void Renderer::Update()
{
}