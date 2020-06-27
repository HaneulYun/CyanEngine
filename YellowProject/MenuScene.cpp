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

	ASSET AddTexture("girltex", L"girl.dds");	
	ASSET AddTexture("boytex", L"boy.dds");	
	ASSET AddTexture("Effecttex", L"Effect.dds");
	ASSET AddTexture("PokemonCentertex", L"pokemonCenter.dds");
	ASSET AddTexture("collidertex", L"collider.dds");
	ASSET AddTexture("housetex", L"house.dds");
	ASSET AddTexture("treetex", L"tree.dds");
	ASSET AddTexture("roadtex", L"road.dds");

	//*** Material ***//
	ASSET AddMaterial("none", ASSET TEXTURE("none"));
	ASSET AddMaterial("black", ASSET TEXTURE("none"), -1, { 0.0, 0.0, 0.0, 1 });
	ASSET AddMaterial("chessmapmat", ASSET TEXTURE("chessmaptex"), -1, { 0.8, 0.8, 0.8, 1 });
	ASSET AddMaterial("mychessmat", ASSET TEXTURE("mychesstex"), -1, { 0.8, 0.8, 0.8, 1 });	
	ASSET AddMaterial("otherchessmat", ASSET TEXTURE("otherchesstex"), -1, { 0.8, 0.8, 0.8, 1 });
	ASSET AddMaterial("kittymat", ASSET TEXTURE("kittytex"), -1, { 0.8, 0.8, 0.8, 1 });
	ASSET AddMaterial("pink", ASSET TEXTURE("none"), -1, { 1.0, 0.8, 0.8, 1 });

	ASSET AddMaterial("girlmat", ASSET TEXTURE("girltex"), -1, { 0.8, 0.8, 0.8, 1 });
	ASSET AddMaterial("boymat", ASSET TEXTURE("boytex"), -1, { 0.8, 0.8, 0.8, 1 });
	ASSET AddMaterial("Effectmat", ASSET TEXTURE("Effecttex"), -1, { 0.8, 0.8, 0.8, 1 });
	ASSET AddMaterial("PokemonCentermat", ASSET TEXTURE("PokemonCentertex"), -1, { 0.8, 0.8, 0.8, 1 });
	ASSET AddMaterial("collidermat", ASSET TEXTURE("collidertex"), -1, { 0.8, 0.8, 0.8, 1 });
	ASSET AddMaterial("housemat", ASSET TEXTURE("housetex"), -1, { 0.8, 0.8, 0.8, 1 });
	ASSET AddMaterial("treemat", ASSET TEXTURE("treetex"), -1, { 0.8, 0.8, 0.8, 1 });
	ASSET AddMaterial("roadmat", ASSET TEXTURE("roadtex"), -1, { 0.8, 0.8, 0.8, 1 });

	//*** Mesh ***//
	ASSET AddMesh("Image", Mesh::CreateQuad());
	ASSET AddMesh("Sphere", Mesh::CreateSphere());
	ASSET AddMesh("Plane", Mesh::CreatePlane());
	ASSET AddMesh("Quad", Mesh::CreateQuad());

	///*** Game Object ***///

	GameObject* chessmap[2][2];
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
		{
			chessmap[i][j] = CreateEmpty();
			chessmap[i][j]->GetComponent<Transform>()->position = { 400.f*j, -399.f - (400.f * i), 0.0f };// { -0.0275f + (j * 22.f), -21.9725f - (i * 22.f), 0.0f };
			chessmap[i][j]->GetComponent<Transform>()->Scale({ 400.0f, 400.0f,1.0f });
			auto mesh = chessmap[i][j]->AddComponent<MeshFilter>()->mesh = ASSET MESH("Quad");
			auto renderer = chessmap[i][j]->AddComponent<Renderer>();
			for (auto& sm : mesh->DrawArgs)
				renderer->materials.push_back(ASSET MATERIAL("roadmat"));
			chessmap[i][j]->layer = (int)RenderLayer::Opaque;
		}



	auto mychess = CreateEmpty();
	{
		mychess->GetComponent<Transform>()->position = { 0.f, 0.f, -0.0001f };
		mychess->GetComponent<Transform>()->Scale({ 1.0f, 1.0f,1.0f });
		auto mesh = mychess->AddComponent<MeshFilter>()->mesh = ASSET MESH("Quad");
		auto renderer = mychess->AddComponent<Renderer>();
		for (auto& sm : mesh->DrawArgs)
			renderer->materials.push_back(ASSET MATERIAL("girlmat"));
		mychess->layer = (int)RenderLayer::Opaque;
		mychess->AddComponent<CharacterController>()->player = true;

		auto name = CreateUI();
		{
			mychess->AddChildUI(name);
			auto rect = name->GetComponent<RectTransform>();
			rect->width = 200;
			rect->height = 50;
			rect->pivot = { 0.5, 0.5 };

			auto text = name->AddComponent<Text>();
			text->text = L"";
			text->fontSize = 10;
			text->color = { 1.0f, 1.0f, 0.0f, 1 };
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
		}

		GameObject* attackEffect[4];
		{
			for (int i = 0; i < 4; ++i)
			{
				attackEffect[i] = mychess->AddChild();
				attackEffect[i]->GetComponent<Transform>()->Scale({ 1.0f, 1.0f,1.0f });
				auto mesh = attackEffect[i]->AddComponent<MeshFilter>()->mesh = ASSET MESH("Quad");
				mychess->GetComponent<CharacterController>()->attackEffect[i] = attackEffect[i];
				auto renderer = attackEffect[i]->AddComponent<Renderer>();
				for (auto& sm : mesh->DrawArgs)
					renderer->materials.push_back(ASSET MATERIAL("Effectmat"));
				attackEffect[i]->layer = (int)RenderLayer::Opaque;
				mychess->GetComponent<CharacterController>()->attackEffect[i]->SetActive(false);
			}
			attackEffect[0]->GetComponent<Transform>()->position = { -1.f, 0.f, -0.0001f };
			attackEffect[1]->GetComponent<Transform>()->position = { 1.f, 0.f, -0.0001f };
			attackEffect[2]->GetComponent<Transform>()->position = { 0.f, -1.f, -0.0001f };
			attackEffect[3]->GetComponent<Transform>()->position = { 0.f, 1.f, -0.0001f };
		}

		auto cameraOffset = mychess->AddChild();
		{
			camera = camera->main = cameraOffset->AddComponent<Camera>();
			cameraOffset->GetComponent<Transform>()->position = { 0.f, 0.0f, -17.3f };
		}
	}

	auto otherchessprefab = CreateEmptyPrefab();
	{
		otherchessprefab->GetComponent<Transform>()->position = { 0.f, 0.f, -0.0001f };
		otherchessprefab->GetComponent<Transform>()->Scale({ 1.0f, 1.0f,1.0f });
		auto mesh = otherchessprefab->AddComponent<MeshFilter>()->mesh = ASSET MESH("Quad");
		auto renderer = otherchessprefab->AddComponent<Renderer>();
		for (auto& sm : mesh->DrawArgs)
			renderer->materials.push_back(ASSET MATERIAL("boymat"));
		otherchessprefab->layer = (int)RenderLayer::Opaque;
		otherchessprefab->AddComponent<CharacterController>()->player = false;

		auto name = CreateUIPrefab();
		{
			otherchessprefab->AddChildUI(name);
			auto rect = name->GetComponent<RectTransform>();
			rect->width = 200;
			rect->height = 50;
			rect->pivot = { 0.5, 0.5 };

			auto text = name->AddComponent<Text>();
			text->text = L"";
			text->fontSize = 10;
			text->color = { 1.0f, 1.0f, 0.0f, 1 };
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
		}
	}

	auto npcchessprefab = CreateEmptyPrefab();
	{
		npcchessprefab->GetComponent<Transform>()->position = { 0.f, 0.f, -0.0001f };
		npcchessprefab->GetComponent<Transform>()->Scale({ 1.0f, 1.0f,1.0f });
		auto mesh = npcchessprefab->AddComponent<MeshFilter>()->mesh = ASSET MESH("Quad");
		auto renderer = npcchessprefab->AddComponent<Renderer>();
		for (auto& sm : mesh->DrawArgs)
			renderer->materials.push_back(ASSET MATERIAL("kittymat"));
		npcchessprefab->layer = (int)RenderLayer::Opaque;
		npcchessprefab->AddComponent<CharacterController>()->player = false;

		auto name = CreateUIPrefab();
		{
			npcchessprefab->AddChildUI(name);
			auto rect = name->GetComponent<RectTransform>();
			rect->width = 200;
			rect->height = 50;
			rect->pivot = { 0.5, 0.5 };

			auto text = name->AddComponent<Text>();
			text->text = L"";
			text->fontSize = 10;
			text->color = { 1.0f, 1.0f, 0.0f, 1 };
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
		}
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
		rectTransform->posY = 0;
		rectTransform->width = 80;
		rectTransform->height = 20;

		ServerButton->AddComponent<Button>()->AddEvent(
			[](void*) {
				Network::network->PressButton();
			});
		{

			Text* text = ServerButton->AddComponent<Text>();
			text->text = L"connect";
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			textObjects.push_back(ServerButton);
		}
	}

	auto logoutButton = CreateImage();
	{
		auto rectTransform = logoutButton->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0, 1 };
		rectTransform->anchorMax = { 0, 1 };
		rectTransform->pivot = { 0, 1 };
		rectTransform->posX = 320;
		rectTransform->posY = 0;
		rectTransform->width = 80;
		rectTransform->height = 20;

		logoutButton->AddComponent<Button>()->AddEvent(
			[](void*) {
				Network::network->Logout();
			});
		{
			Text* text = logoutButton->AddComponent<Text>();
			text->text = L"logout";
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			textObjects.push_back(logoutButton);
		}
	}

	auto coordinate = CreateImage();
	{
		auto rectTransform = coordinate->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0, 1 };
		rectTransform->anchorMax = { 0, 1 };
		rectTransform->pivot = { 0, 1 };
		rectTransform->posX = 0;
		rectTransform->posY = -20;
		rectTransform->width = 80;
		rectTransform->height = 20;

		{
			Text* text = coordinate->AddComponent<Text>();
			text->text = L"(0, 30)";
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			textObjects.push_back(coordinate);

			mychess->GetComponent<CharacterController>()->coord = text;
		}
	}

	auto levelandhp = CreateImage();
	{
		auto rectTransform = levelandhp->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0, 1 };
		rectTransform->anchorMax = { 0, 1 };
		rectTransform->pivot = { 0, 1 };
		rectTransform->posX = 100;
		rectTransform->posY = 0;
		rectTransform->width = 200;
		rectTransform->height = 20;

		{
			Text* text = levelandhp->AddComponent<Text>();
			text->text = L"Level: 0, EXP: 0, HP: 0";
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			textObjects.push_back(levelandhp);

			mychess->GetComponent<CharacterController>()->levelandhp = text;
		}
	}

	GameObject* chatting[5];
	for (int i = 0; i < 5; ++i)
	{
		chatting[i] = CreateImage();
		auto rectTransform = chatting[i]->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0, 1 };
		rectTransform->anchorMax = { 0, 1 };
		rectTransform->pivot = { 0, 1 };
		rectTransform->posX = 0;
		rectTransform->posY = -380 + (i * 20);
		rectTransform->width = 300;
		rectTransform->height = 20;

		{
			Text* text = chatting[i]->AddComponent<Text>();
			text->text = L"";
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			textObjects.push_back(chatting[i]);

			if (i != 0)
			{
				auto renderer = chatting[i]->GetComponent<Renderer>();
				renderer->materials[0] = ASSET MATERIAL("pink");

				Network::network->chatText[i - 1] = text;
			}
			else
			{
				chatting[i]->AddComponent<Button>()->AddEvent(
					[](void*) {
						Network::network->PressChatButton();
					});
				Network::network->chatInputText = text;
			}
		}
	}
}