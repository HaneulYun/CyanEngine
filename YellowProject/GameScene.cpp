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

	//*** Material ***//
	ASSET AddMaterial("none", ASSET TEXTURE("none"));
	ASSET AddMaterial("boardMat", ASSET TEXTURE("boardTex"));
	ASSET AddMaterial("blackTileMat", ASSET TEXTURE("blackTileTex"));
	ASSET AddMaterial("whiteTileMat", ASSET TEXTURE("whiteTileTex"));
	ASSET AddMaterial("pawnMat", ASSET TEXTURE("pawnTex"));

	//*** Mesh ***//
	ASSET AddMesh("Image", Mesh::CreateQuad());
	ASSET AddMesh("Plane", Mesh::CreatePlane());

	///*** Game Object ***///

	auto mainCamera = CreateEmpty();
	{
		mainCamera->transform->position = { 7.5, 7.5, -10 };
		camera = camera->main = mainCamera->AddComponent<Camera>();
		camera->GenerateOrthoMatrix(16, 16, 0, 100);
		
	}

	GameObject* prefab = CreateEmptyPrefab();
	{
		prefab->transform->Rotate({ 1, 0, 0 }, -90);
		prefab->transform->position = { 0, 0, -1 };
		prefab->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		prefab->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("pawnMat"));

		prefab->AddComponent<Pawn>()->x = -1;
		prefab->GetComponent<Pawn>()->y = -1;
	}

	GameObject* board = CreateEmpty();
	{
		board->transform->Scale({ 8, 8, 1 });
		board->transform->Rotate({ 1, 0, 0 }, -90);
		//board->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		//board->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("boardMat"));
	
		board->AddComponent<TCPClient>()->prefab = prefab;
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