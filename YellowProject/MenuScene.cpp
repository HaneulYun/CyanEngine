#include "pch.h"
#include "MenuScene.h"

void MenuScene::BuildObjects()
{
	///*** Asset ***///
	//*** Texture ***//
	ASSET AddTexture("none", L"Textures\\none.dds");
	ASSET AddTexture("chessmaptex", L"chessmap.dds");
	ASSET AddTexture("mychesstex", L"mychess.dds");
	ASSET AddTexture("otherchesstex", L"otherchess.dds");

	//*** Material ***//
	ASSET AddMaterial("none", ASSET TEXTURE("none"));
	ASSET AddMaterial("gray", ASSET TEXTURE("none"), -1, { 0.5, 0.5, 0.5, 0.5 });
	ASSET AddMaterial("chessmapmat", ASSET TEXTURE("chessmaptex"), -1, { 0.8, 0.8, 0.8, 1 });
	ASSET AddMaterial("mychessmat", ASSET TEXTURE("mychesstex"), -1, { 0.8, 0.8, 0.8, 1 });	
	ASSET AddMaterial("otherchessmat", ASSET TEXTURE("otherchesstex"), -1, { 0.8, 0.8, 0.8, 1 });

	//*** Mesh ***//
	ASSET AddMesh("Image", Mesh::CreateQuad());
	ASSET AddMesh("Sphere", Mesh::CreateSphere());
	ASSET AddMesh("Plane", Mesh::CreatePlane());
	///*** Game Object ***///


	auto chessmap = CreateEmpty();
	{	
		chessmap->GetComponent<Transform>()->position = { 14.96f, -14.96f, 0.0f };
		chessmap->GetComponent<Transform>()->Scale({ 3.0f, 3.0f, 3.0f });
		chessmap->GetComponent<Transform>()->Rotate({ 1.0f, 0.0f, 0.0f }, -90);
		auto mesh = chessmap->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		auto renderer = chessmap->AddComponent<Renderer>();
		for (auto& sm : mesh->DrawArgs)
			renderer->materials.push_back(ASSET MATERIAL("chessmapmat"));
		chessmap->layer = (int)RenderLayer::Opaque;
	}

	auto mychess = CreateEmpty();
	{
		mychess->GetComponent<Transform>()->position = { 0.f, 0.f, -0.01f };
		mychess->GetComponent<Transform>()->Scale({ 0.0075, 0.0075,1.0f });
		mychess->GetComponent<Transform>()->Rotate({ 1.0f, 0.0f, 0.0f }, -90);
		auto mesh = mychess->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		auto renderer = mychess->AddComponent<Renderer>();
		for (auto& sm : mesh->DrawArgs)
			renderer->materials.push_back(ASSET MATERIAL("mychessmat"));
		mychess->layer = (int)RenderLayer::Opaque;
		mychess->AddComponent<CharacterController>();

		auto cameraOffset = mychess->AddChild();
		{
			camera = camera->main = cameraOffset->AddComponent<Camera>();
			cameraOffset->GetComponent<Transform>()->Rotate({ 1.0f, 0.0f, 0.0f }, 90);
			cameraOffset->GetComponent<Transform>()->position = { 0.f, 0.99f, -0.99f };
		
			cameraOffset->AddComponent<CameraController>();
		}
	}

	auto otherchessprefab = CreateEmptyPrefab();
	{
		otherchessprefab->GetComponent<Transform>()->position = { 0.f, 0.f, -0.01f };
		otherchessprefab->GetComponent<Transform>()->Scale({ 0.0075, 0.0075,1.0f });
		otherchessprefab->GetComponent<Transform>()->Rotate({ 1.0f, 0.0f, 0.0f }, -90);
		auto mesh = otherchessprefab->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		auto renderer = otherchessprefab->AddComponent<Renderer>();
		for (auto& sm : mesh->DrawArgs)
			renderer->materials.push_back(ASSET MATERIAL("otherchessmat"));
		otherchessprefab->layer = (int)RenderLayer::Opaque;
	}


}