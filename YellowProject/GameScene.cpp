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
	ASSET AddMaterial("gray", ASSET TEXTURE("none"), -1, { 0.5, 0.5, 0.5, 0.5 });
	ASSET AddMaterial("boardMat", ASSET TEXTURE("boardTex"), -1, { 0.8, 0.8, 0.8, 1 });
	ASSET AddMaterial("pawnMat", ASSET TEXTURE("pawnTex"), -1, { 0.8, 0.8, 0.8, 1 });

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

	GameObject* board = CreateEmpty();
	{
		board->transform->Scale({ 600, 600, 1 });
		board->transform->Rotate({ 1, 0, 0 }, -90);
		board->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		board->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("boardMat"));

		board->AddComponent<TCPClient>();
	}

	for (int i = -1; i < 9; ++i)
	{
		GameObject* pawn = CreateEmpty();
		pawn->transform->Scale({ 75, 75, 1 });
		pawn->transform->Rotate({ 1, 0, 0 }, -90);
		pawn->transform->position = { 0, 0, -1 };
		pawn->AddComponent<MeshFilter>()->mesh = ASSET MESH("Plane");
		pawn->AddComponent<Renderer>()->materials.push_back(ASSET MATERIAL("pawnMat"));
	
		pawn->AddComponent<Pawn>()->x = -1;
		pawn->GetComponent<Pawn>()->y = -1;
	
		board->GetComponent<TCPClient>()->pawns.push_back(pawn->GetComponent<Pawn>());
	}
}