#include "pch.h"
#include "MenuScene.h"

void MenuScene::BuildObjects()
{
	///*** Asset ***///
	//*** Texture ***//
	ASSET AddTexture("none", L"Textures\\none.dds");
	ASSET AddTexture("girltex", L"girl.dds");
	ASSET AddTexture("boytex", L"boy.dds");	
	ASSET AddTexture("Effecttex", L"Effect.dds");
	ASSET AddTexture("gameworldtex", L"gameworld.dds");
	ASSET AddTexture("gameworld2tex", L"gameworld2.dds");
	ASSET AddTexture("eeveetex", L"eevee.dds");
	ASSET AddTexture("jolteontex", L"jolteon.dds");
	ASSET AddTexture("flareontex", L"flareon.dds");
	ASSET AddTexture("vaporeontex", L"vaporeon.dds");
	ASSET AddTexture("leafeontex", L"leafeon.dds");
	ASSET AddTexture("glaceontex", L"glaceon.dds");
	ASSET AddTexture("metagrosstex", L"metagross.dds");
	ASSET AddTexture("mesprittex", L"mesprit.dds");
	ASSET AddTexture("giratinatex", L"giratinatex.dds");
	ASSET AddTexture("arceustex", L"arceus.dds");
	
	//*** Material ***//
	ASSET AddMaterial("none", ASSET TEXTURE("none"));
	ASSET AddMaterial("black", ASSET TEXTURE("none"), -1, { 0.0, 0.0, 0.0, 1 });
	ASSET AddMaterial("pink", ASSET TEXTURE("none"), -1, { 1.0, 0.8, 0.8, 1 });

	ASSET AddMaterial("girlmat", ASSET TEXTURE("girltex"), -1, { 1.0,1.0,1.0, 1 });
	ASSET AddMaterial("boymat", ASSET TEXTURE("boytex"), -1, { 1.0,1.0,1.0, 1 });
	ASSET AddMaterial("Effectmat", ASSET TEXTURE("Effecttex"), -1, { 1.0,1.0,1.0, 1 });
	ASSET AddMaterial("gameworldmat", ASSET TEXTURE("gameworldtex"), -1, { 1.0,1.0,1.0, 1 });
	ASSET AddMaterial("gameworld2mat", ASSET TEXTURE("gameworld2tex"), -1, { 1.0,1.0,1.0, 1 });
	
	ASSET AddMaterial("eeveemat", ASSET TEXTURE("eeveetex"), -1, { 1.0,1.0,1.0, 1 });
	ASSET AddMaterial("jolteonmat", ASSET TEXTURE("jolteontex"), -1, { 1.0,1.0,1.0, 1 });
	ASSET AddMaterial("flareonmat", ASSET TEXTURE("flareontex"), -1, { 1.0,1.0,1.0, 1 });
	ASSET AddMaterial("vaporeonmat", ASSET TEXTURE("vaporeontex"), -1, { 1.0,1.0,1.0, 1 });
	ASSET AddMaterial("leafeonmat", ASSET TEXTURE("leafeontex"), -1, { 1.0,1.0,1.0, 1 });
	ASSET AddMaterial("glaceonmat", ASSET TEXTURE("glaceontex"), -1, { 1.0,1.0,1.0, 1 });
	ASSET AddMaterial("metagrossmat", ASSET TEXTURE("metagrosstex"), -1, { 1.0,1.0,1.0, 1 });
	ASSET AddMaterial("mespritmat", ASSET TEXTURE("mesprittex"), -1, { 1.0,1.0,1.0, 1 });
	ASSET AddMaterial("giratinamat", ASSET TEXTURE("giratinatex"), -1, { 1.0,1.0,1.0, 1 });
	ASSET AddMaterial("arceusmat", ASSET TEXTURE("arceustex"), -1, { 1.0,1.0,1.0, 1 });

	//*** Mesh ***//
	ASSET AddMesh("Image", Mesh::CreateQuad());
	ASSET AddMesh("Sphere", Mesh::CreateSphere());
	ASSET AddMesh("Plane", Mesh::CreatePlane());
	ASSET AddMesh("Quad", Mesh::CreateQuad());

	///*** Game Object ***///

	GameObject* chessmap[WORLD_HEIGHT/40][WORLD_WIDTH/40];
	for (int i = 0; i < WORLD_HEIGHT / 40; ++i)
		for (int j = 0; j < WORLD_WIDTH / 40; ++j)
		{
			chessmap[i][j] = CreateEmpty();
			chessmap[i][j]->GetComponent<Transform>()->position = { 40.f*j, -39.f - (40.f * i), 0.0f };// { -0.0275f + (j * 22.f), -21.9725f - (i * 22.f), 0.0f };
			chessmap[i][j]->GetComponent<Transform>()->Scale({ 40.0f, 40.0f,1.0f });
			auto mesh = chessmap[i][j]->AddComponent<MeshFilter>()->mesh = ASSET MESH("Quad");
			auto renderer = chessmap[i][j]->AddComponent<Renderer>();
			for (auto& sm : mesh->DrawArgs)
			{
				if (i == 0 && j == 0)
					renderer->materials.push_back(ASSET MATERIAL("gameworld2mat"));
				else
					renderer->materials.push_back(ASSET MATERIAL("gameworldmat"));
			}
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

	GameObject* monsterprefab[10];
	for (int i = 0; i < 10; ++i)
	{
		monsterprefab[i] = CreateEmptyPrefab();
		monsterprefab[i]->GetComponent<Transform>()->position = { 0.f, 0.f, -0.0001f };
		monsterprefab[i]->GetComponent<Transform>()->Scale({ 1.0f, 1.0f,1.0f });
		auto mesh = monsterprefab[i]->AddComponent<MeshFilter>()->mesh = ASSET MESH("Quad");
		auto renderer = monsterprefab[i]->AddComponent<Renderer>();
		for (auto& sm : mesh->DrawArgs)
		{
			if (i == 0)
				renderer->materials.push_back(ASSET MATERIAL("eeveemat"));
			else if (i == 1)
				renderer->materials.push_back(ASSET MATERIAL("jolteonmat"));
			else if (i == 2)
				renderer->materials.push_back(ASSET MATERIAL("flareonmat"));
			else if (i == 3)
				renderer->materials.push_back(ASSET MATERIAL("vaporeonmat"));
			else if (i == 4)
				renderer->materials.push_back(ASSET MATERIAL("leafeonmat"));
			else if (i == 5)
				renderer->materials.push_back(ASSET MATERIAL("glaceonmat"));
			else if (i == 6)
				renderer->materials.push_back(ASSET MATERIAL("metagrossmat"));
			else if (i == 7)
				renderer->materials.push_back(ASSET MATERIAL("mespritmat"));
			else if (i == 8)
				renderer->materials.push_back(ASSET MATERIAL("giratinamat"));
			else
				renderer->materials.push_back(ASSET MATERIAL("arceusmat"));
		}

		monsterprefab[i]->layer = (int)RenderLayer::Opaque;
		monsterprefab[i]->AddComponent<CharacterController>()->player = false;

		auto name = CreateUIPrefab();
		{
			monsterprefab[i]->AddChildUI(name);
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
		for (int i = 0; i < 10; ++i)
			network->npcsPrefab[i] = monsterprefab[i];
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
		rectTransform->width = 350;
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