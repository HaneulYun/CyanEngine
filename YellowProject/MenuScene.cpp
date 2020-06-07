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
	ASSET AddTexture("kittytex", L"kitty.dds");

	//*** Material ***//
	ASSET AddMaterial("none", ASSET TEXTURE("none"));
	ASSET AddMaterial("black", ASSET TEXTURE("none"), -1, { 0.0, 0.0, 0.0, 1 });
	ASSET AddMaterial("chessmapmat", ASSET TEXTURE("chessmaptex"), -1, { 0.8, 0.8, 0.8, 1 });
	ASSET AddMaterial("mychessmat", ASSET TEXTURE("mychesstex"), -1, { 0.8, 0.8, 0.8, 1 });	
	ASSET AddMaterial("otherchessmat", ASSET TEXTURE("otherchesstex"), -1, { 0.8, 0.8, 0.8, 1 });
	ASSET AddMaterial("kittymat", ASSET TEXTURE("kittytex"), -1, { 0.8, 0.8, 0.8, 1 });

	//*** Mesh ***//
	ASSET AddMesh("Image", Mesh::CreateQuad());
	ASSET AddMesh("Sphere", Mesh::CreateSphere());
	ASSET AddMesh("Plane", Mesh::CreatePlane());
	ASSET AddMesh("Quad", Mesh::CreateQuad());

	///*** Game Object ***///

	auto chessmap = CreateEmpty();
	{
		chessmap = CreateEmpty();
		chessmap->GetComponent<Transform>()->position = { -0.0275f, -21.9725f, 0.0f };
		chessmap->GetComponent<Transform>()->Scale({ 22.0f, 22.0f,1.0f });
		auto mesh = chessmap->AddComponent<MeshFilter>()->mesh = ASSET MESH("Quad");
		auto renderer = chessmap->AddComponent<Renderer>();
		for (auto& sm : mesh->DrawArgs)
			renderer->materials.push_back(ASSET MATERIAL("chessmapmat"));
		chessmap->layer = (int)RenderLayer::Opaque;
	}

	auto mychess = CreateEmpty();
	{
		mychess->GetComponent<Transform>()->position = { 0.f, 0.f, -0.0001f };
		mychess->GetComponent<Transform>()->Scale({ 0.005625, 0.005625,1.0f });
		mychess->GetComponent<Transform>()->Rotate({ 1.0f, 0.0f, 0.0f }, -90);
		auto mesh = mychess->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		auto renderer = mychess->AddComponent<Renderer>();
		for (auto& sm : mesh->DrawArgs)
			renderer->materials.push_back(ASSET MATERIAL("mychessmat"));
		mychess->layer = (int)RenderLayer::Opaque;
		mychess->AddComponent<CharacterController>()->player = true;

		auto cameraOffset = mychess->AddChild();
		{
			camera = camera->main = cameraOffset->AddComponent<Camera>();
			cameraOffset->GetComponent<Transform>()->Rotate({ 1.0f, 0.0f, 0.0f }, 90);
			cameraOffset->GetComponent<Transform>()->position = { 0.f, 0.9999f, -0.9999f };
		}
	}

	auto otherchessprefab = CreateEmptyPrefab();
	{
		otherchessprefab->GetComponent<Transform>()->position = { 0.f, 0.f, -0.0001f };
		otherchessprefab->GetComponent<Transform>()->Scale({ 0.005625, 0.005625,1.0f });
		otherchessprefab->GetComponent<Transform>()->Rotate({ 1.0f, 0.0f, 0.0f }, -90);
		auto mesh = otherchessprefab->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		auto renderer = otherchessprefab->AddComponent<Renderer>();
		for (auto& sm : mesh->DrawArgs)
			renderer->materials.push_back(ASSET MATERIAL("otherchessmat"));
		otherchessprefab->layer = (int)RenderLayer::Opaque;
		otherchessprefab->AddComponent<CharacterController>()->player = false;
	}

	auto npcchessprefab = CreateEmptyPrefab();
	{
		npcchessprefab->GetComponent<Transform>()->position = { 0.f, 0.f, -0.0001f };
		npcchessprefab->GetComponent<Transform>()->Scale({ 0.005625, 0.005625,1.0f });
		npcchessprefab->GetComponent<Transform>()->Rotate({ 1.0f, 0.0f, 0.0f }, -90);
		auto mesh = npcchessprefab->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		auto renderer = npcchessprefab->AddComponent<Renderer>();
		for (auto& sm : mesh->DrawArgs)
			renderer->materials.push_back(ASSET MATERIAL("kittymat"));
		npcchessprefab->layer = (int)RenderLayer::Opaque;
		npcchessprefab->AddComponent<CharacterController>()->player = false;
	}

	auto manager = CreateEmpty();
	{
		Network* network = manager->AddComponent<Network>();
		network->othersPrefab = otherchessprefab;
		network->myCharacter = mychess;
		network->npcsPrefab = npcchessprefab;
		Network::network = network;
	}

	auto ServerButton = CreateImage();
	{
		auto rectTransform = ServerButton->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0, 1 };
		rectTransform->anchorMax = { 0, 1 };
		rectTransform->pivot = { 0, 1 };
		rectTransform->posX = 0;
		rectTransform->posY = -10;
		rectTransform->width = 80;
		rectTransform->height = 30;

		ServerButton->AddComponent<Button>()->AddEvent(
			[](void*) {
				Network::network->PressButton();
			});
		{
			//auto textobject = ServerButton->AddChildUI();
			//auto rectTransform = textobject->GetComponent<RectTransform>();
			//rectTransform->anchorMin = { 0, 0 };
			//rectTransform->anchorMax = { 1, 1 };

			Text* text = ServerButton->AddComponent<Text>();
			text->text = L"connect";
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			textObjects.push_back(ServerButton);
		}
	}
}