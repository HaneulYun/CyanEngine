#include "pch.h"
#include "Renderer.h"

Renderer::Renderer()
{
	graphics = Graphics::Instance();
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

	{
		auto item = std::make_unique<RenderItem>();

		Vector3 position = gameObject->GetComponent<Transform>()->position;
		Vector3 scale = gameObject->GetComponent<Transform>()->localScale;

		XMStoreFloat4x4(&item->World, XMMatrixScaling(scale.x, scale.y, scale.z) * XMMatrixTranslation(position.x, position.y, position.z));
		XMStoreFloat4x4(&item->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
		item->ObjCBIndex = Scene::scene->allRItems.size();
		item->Mat = material;
		item->Geo = meshFilter->mesh;
		item->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		item->IndexCount = item->Geo->DrawArgs["submesh"].IndexCount;
		item->StartIndexLocation = item->Geo->DrawArgs["submesh"].StartIndexLocation;
		item->BaseVertexLocation = item->Geo->DrawArgs["submesh"].BaseVertexLocation;

		Scene::scene->renderItemLayer[(int)RenderLayer::Opaque].push_back(item.get());
		Scene::scene->allRItems.push_back(std::move(item));
	}
}

void Renderer::Update()
{
}