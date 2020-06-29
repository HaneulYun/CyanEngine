#include "pch.h"
#include "GameScene.h"

void GameScene::BuildObjects()
{
	///*** Asset ***///
	//*** Texture ***//
	ASSET AddTexture("none", L"Textures\\none.dds");
	ASSET AddTexture("blackTileTex", L"Textures\\black_tile.dds");
	ASSET AddTexture("whiteTileTex", L"Textures\\white_tile.dds");
	ASSET AddTexture("playerTex", L"Textures\\p2.dds");
	ASSET AddTexture("userTex", L"Textures\\p1.dds");
	ASSET AddTexture("npcTex", L"Textures\\p5.dds");

	{
		std::string texName = "tileTex";
		std::wstring filePath = L"Textures\\mapimage\\";
		std::wstring dds = L".dds";
		for (int i = 0; i < 104; ++i)
			ASSET AddTexture(texName + std::to_string(i), filePath + std::to_wstring(i) + dds);
	}

	//*** Material ***//
	ASSET AddMaterial("none", ASSET TEXTURE("none"));
	ASSET AddMaterial("blackTileMat", ASSET TEXTURE("blackTileTex"));
	ASSET AddMaterial("whiteTileMat", ASSET TEXTURE("whiteTileTex"));
	ASSET AddMaterial("playerMat", ASSET TEXTURE("playerTex"));
	ASSET AddMaterial("userMat", ASSET TEXTURE("userTex"));
	ASSET AddMaterial("npcMat", ASSET TEXTURE("npcTex"));
	{
		std::string texName = "tileTex";
		for (int i = 0; i < 104; ++i)
			ASSET AddMaterial(std::to_string(i), ASSET TEXTURE(texName + std::to_string(i)));
	}

	//*** Mesh ***//
	ASSET AddMesh("Image", Mesh::CreateQuad());
	ASSET AddMesh("Plane", Mesh::CreatePlane());

	///*** Game Object ***///

	auto mainCamera = CreateEmpty();
	{
		mainCamera->transform->position = { 9.5, 9.5, -10 };
		camera = camera->main = mainCamera->AddComponent<Camera>();
		camera->GenerateOrthoMatrix(20, 20, 0, 100);
		
	}

	GameObject* coordinatePrinter = CreateUI();
	{
		auto rect = coordinatePrinter->GetComponent<RectTransform>();
		rect->anchorMin = { 0.5, 1.0 };
		rect->anchorMax = { 0.5, 1.0 };
		rect->pivot = { 0.5, 1.0 };
		rect->width = 200;
		rect->height = 200;

		auto text = coordinatePrinter->AddComponent<Text>();
		text->text = L"(0, 0)";
		text->font = L"°íµñ";
		text->fontSize = 40;
		text->color = { 217 / 255.f, 120 / 255.f, 235 / 255.f, 1 };
		text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
	}

	GameObject* prefab = CreateEmptyPrefab();
	{
		prefab->transform->Rotate({ 1, 0, 0 }, -90);
		prefab->transform->position = { 0, 0, -1 };
		prefab->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		prefab->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("playerMat"));

		prefab->AddComponent<Pawn>()->x = -1;
		prefab->GetComponent<Pawn>()->y = -1;

		GameObject* name = CreateUIPrefab();
		{
			prefab->AddChildUI(name);
			auto rect = name->GetComponent<RectTransform>();
			rect->width = 50;
			rect->height = 50;
			rect->pivot = { 0.5, 0.5 };

			auto text = name->AddComponent<Text>();
			text->text = L"";
			text->font = L"°íµñ";
			text->fontSize = 15;
			text->color = { 235 / 255.f, 207 / 255.f, 73 / 255.f, 1 };
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
		}

		GameObject* chat = CreateUIPrefab();
		{
			prefab->AddChildUI(chat);
			auto rect = chat->GetComponent<RectTransform>();
			rect->width = 50;
			rect->height = 50;
			rect->pivot = { 0.5, 0.5 };

			auto text = chat->AddComponent<Text>();
			text->text = L"";
			text->font = L"°íµñ";
			text->fontSize = 15;
			text->color = { 87 / 255.f, 120 / 255.f, 207 / 255.f, 1 };
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
		}
	}

	auto TextEditForID = CreateUI();
	{
		auto rect = TextEditForID->GetComponent<RectTransform>();
		rect->anchorMin = { 0.5, 0.8 };
		rect->anchorMax = { 0.5, 0.8 };
		rect->width = 400;
		rect->height = 100;

		auto text = TextEditForID->AddComponent<Text>();
		text->text = L"ID : (input here)";
		text->font = L"°íµñ";
		text->fontSize = 40;
		text->color = { 217 / 255.f, 120 / 255.f, 235 / 255.f, 1 };
		text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
	}

	auto TextEditForIP = CreateUI();
	{
		auto rect = TextEditForIP->GetComponent<RectTransform>();
		rect->anchorMin = { 0.5, 0.7 };
		rect->anchorMax = { 0.5, 0.7 };
		rect->width = 400;
		rect->height = 100;

		auto text = TextEditForIP->AddComponent<Text>();
		text->text = L"IP : (input here)";
		text->font = L"°íµñ";
		text->fontSize = 40;
		text->color = { 217 / 255.f, 120 / 255.f, 235 / 255.f, 1 };
		text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
	}

	GameObject* board = CreateEmpty();
	{
		board->transform->Scale({ 8, 8, 1 });
		board->transform->Rotate({ 1, 0, 0 }, -90);
	
		board->AddComponent<TCPClient>()->prefab = prefab;
		board->GetComponent<TCPClient>()->coordinateText = coordinatePrinter->GetComponent<Text>();
		board->GetComponent<TCPClient>()->idEditor = TextEditForID;
		board->GetComponent<TCPClient>()->ipEditor = TextEditForIP;
	}

	for (int i = 0; i < SCREEN_WIDTH; ++i)
		for (int j = 0; j < SCREEN_HEIGHT; ++j)
		{
			GameObject* tile = CreateEmpty();
			board->GetComponent<TCPClient>()->tiles[j][i] = tile;
			{
				tile->transform->Rotate({ 1, 0, 0 }, -90);
				tile->transform->position = { (float)i, (float)j, 0};
				tile->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
				//if ((i / 3 + j / 3) % 2)
				if ((i + j) % 2)
					tile->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("whiteTileMat"));
				else
					tile->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("blackTileMat"));
			}
		}

	auto status = CreateImage();
	{
		status->GetComponent<Renderer>()->materials[0] = ASSET MATERIAL("whiteTileMat");

		auto rectTransform = status->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0, 1 };
		rectTransform->anchorMax = { 0, 1 };
		rectTransform->pivot = { 0, 1 };
		rectTransform->posX = 8;
		rectTransform->posY = -8;
		rectTransform->width = 240;
		rectTransform->height = 50;

		auto text = status->AddComponent<Text>();
		text->text = L" Level : 1        Exp : 0 / 100 \n HP : 100 / 100";
		text->font = L"°íµñ";
		text->fontSize = 20;
		text->color = { 1, 1, 1, 1 };
		textObjects.push_back(status);
		board->GetComponent<TCPClient>()->statusText = text;
	}

	auto chatGUI = CreateImage();
	{
		chatGUI->GetComponent<Renderer>()->materials[0] = ASSET MATERIAL("whiteTileMat");

		auto rectTransform = chatGUI->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0, 0 };
		rectTransform->anchorMax = { 0, 0 };
		rectTransform->pivot = { 0, 0 };
		rectTransform->posX = 8;
		rectTransform->posY = 32;
		rectTransform->width = 400 - 8;
		rectTransform->height = 200 - 32;

		auto text = chatGUI->AddComponent<Text>();
		text->text = L"";
		text->font = L"°íµñ";
		text->fontSize = 20;
		text->color = { 1, 1, 1, 1 };
		text->paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
		textObjects.push_back(chatGUI);
		board->GetComponent<TCPClient>()->logger = text;
	}

	auto chatInputter = CreateImage();
	{
		chatInputter->GetComponent<Renderer>()->materials[0] = ASSET MATERIAL("whiteTileMat");

		auto rectTransform = chatInputter->GetComponent<RectTransform>();
		rectTransform->anchorMin = { 0, 0 };
		rectTransform->anchorMax = { 0, 0 };
		rectTransform->pivot = { 0, 0 };
		rectTransform->posX = 8;
		rectTransform->posY = 8;
		rectTransform->width = 400 - 8;
		rectTransform->height = 22;

		auto text = chatInputter->AddComponent<Text>();
		text->text = L"";
		text->font = L"°íµñ";
		text->fontSize = 20;
		text->color = { 1, 1, 1, 1 };
		textObjects.push_back(chatInputter);
		board->GetComponent<TCPClient>()->chatter = text;
	}
}