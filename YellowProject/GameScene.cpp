#include "pch.h"
#include "GameScene.h"

void GameScene::BuildObjects()
{
	///*** Asset ***///
	//*** Texture ***//
	ASSET AddTexture("none", L"Textures\\none.dds");
	ASSET AddTexture("boardTex", L"Textures\\board.dds");
	ASSET AddTexture("pawnTex", L"Textures\\pawn.dds");

	//*** Material ***//
	ASSET AddMaterial("none", ASSET TEXTURE("none"));
	ASSET AddMaterial("boardMat", ASSET TEXTURE("boardTex"));
	ASSET AddMaterial("pawnMat", ASSET TEXTURE("pawnTex"));

	//*** Mesh ***//
	ASSET AddMesh("Image", Mesh::CreateQuad());
	ASSET AddMesh("Plane", Mesh::CreatePlane());

	///*** Game Object ***///

	auto mainCamera = CreateEmpty();
	{
		mainCamera->transform->position = { 0, 0, -10 };
		camera = camera->main = mainCamera->AddComponent<Camera>();
		camera->GenerateOrthoMatrix(800, 600, 0, 100);
	}

	GameObject* prefab = CreateEmptyPrefab();
	{
		prefab->transform->Scale({ 75, 75, 1 });
		prefab->transform->Rotate({ 1, 0, 0 }, -90);
		prefab->transform->position = { 0, 0, -1 };
		prefab->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		prefab->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("pawnMat"));

		prefab->AddComponent<Pawn>()->x = -1;
		prefab->GetComponent<Pawn>()->y = -1;
	}

	GameObject* board = CreateEmpty();
	{
		board->transform->Scale({ 600, 600, 1 });
		board->transform->Rotate({ 1, 0, 0 }, -90);
		board->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		board->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("boardMat"));

		board->AddComponent<TCPClient>()->prefab = prefab;
	}
}