#include "pch.h"
#include "GameScene.h"

void GameScene::BuildObjects()
{
	///*** Asset ***///
	//*** Texture ***//
	ASSET AddTexture("none", L"Textures\\none.dds");
	ASSET AddTexture("boardTex", L"Textures\\board.dds");
	ASSET AddTexture("blackTileTex", L"Textures\\black_tile.dds");
	ASSET AddTexture("whiteTileTex", L"Textures\\white_tile.dds");
	ASSET AddTexture("pawnTex", L"Textures\\pawn.dds");
	ASSET AddTexture("playerTex", L"Textures\\p2.dds");
	ASSET AddTexture("userTex", L"Textures\\p1.dds");
	ASSET AddTexture("npcTex", L"Textures\\p5.dds");

	//*** Material ***//
	ASSET AddMaterial("none", ASSET TEXTURE("none"));
	ASSET AddMaterial("boardMat", ASSET TEXTURE("boardTex"));
	ASSET AddMaterial("blackTileMat", ASSET TEXTURE("blackTileTex"));
	ASSET AddMaterial("whiteTileMat", ASSET TEXTURE("whiteTileTex"));
	ASSET AddMaterial("pawnMat", ASSET TEXTURE("pawnTex"));
	ASSET AddMaterial("playerMat", ASSET TEXTURE("playerTex"));
	ASSET AddMaterial("userMat", ASSET TEXTURE("userTex"));
	ASSET AddMaterial("npcMat", ASSET TEXTURE("npcTex"));

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
		text->font = L"메이플스토리";
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
			text->font = L"메이플스토리";
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
			text->font = L"메이플스토리";
			text->fontSize = 15;
			text->color = { 87 / 255.f, 120 / 255.f, 207 / 255.f, 1 };
			text->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
		}
	}

	GameObject* board = CreateEmpty();
	{
		board->transform->Scale({ 8, 8, 1 });
		board->transform->Rotate({ 1, 0, 0 }, -90);
		//board->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		//board->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("boardMat"));
	
		board->AddComponent<TCPClient>()->prefab = prefab;
		board->GetComponent<TCPClient>()->coordinateText = coordinatePrinter->GetComponent<Text>();
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
}