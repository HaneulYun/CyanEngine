#include "pch.h"
#include "SampleScene.h"

Scene* Scene::scene{ nullptr };

void SampleScene::BuildObjects()
{
	scene = this;

	//*** Asset ***//

	Material* material_defaultMaterial = new DefaultMaterial();

	Mesh* mesh_cube = new Cube();
	Mesh* mesh_grid = new Plane();
	Mesh* mesh_sphere = new Sphere();
	Mesh* mesh_cylinder = new Cylinder();

	rendererManager->textureData.push_back({ "boardTex", L"..\\CyanEngine\\Textures\\borad.dds" });
	rendererManager->textureData.push_back({ "pawnTex", L"..\\CyanEngine\\Textures\\pawn.dds" });

	rendererManager->materials["board"] = std::make_unique<Material>();
	rendererManager->materials["pawn"] = std::make_unique<Material>();
	Material* material_board = rendererManager->materials["board"].get();
	Material* material_pawn = rendererManager->materials["pawn"].get();

	{
		material_board->Name = "board";
		material_board->MatCBIndex = 0;
		material_board->DiffuseSrvHeapIndex = 0;
		material_board->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		material_board->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
		material_board->Roughness = 0.1f;

		material_pawn->Name = "pawn";
		material_pawn->MatCBIndex = 1;
		material_pawn->DiffuseSrvHeapIndex = 1;
		material_pawn->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		material_pawn->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
		material_pawn->Roughness = 0.1f;
	}


	//*** Game Object ***//

	GameObject* mainCamera = CreateEmpty();
	{
		Camera* camera = mainCamera->AddComponent<Camera>();
		camera->SetViewport(0, 0, CyanFW::Instance()->GetWidth(), CyanFW::Instance()->GetHeight(), 0.0f, 1.0f);
		camera->SetScissorRect(0, 0, CyanFW::Instance()->GetWidth(), CyanFW::Instance()->GetHeight());
		camera->GenerateOrthoMatrix(800, 600, 0, 100);
		camera->GenerateViewMatrix(XMFLOAT3(0.0f, 0.0f, -15.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		camera->main = camera;
	}

	GameObject* board = CreateEmpty();
	{
		board->transform->Scale({ 60, 60, 60 });
		board->transform->Rotate({ 1, 0, 0 }, -90);
		board->AddComponent<MeshFilter>()->mesh = mesh_grid;
		board->AddComponent<Renderer>()->material = material_board;

		board->AddComponent<TCPClient>();
	}

	for(int i = -1; i < 9; ++i)
	{
		GameObject* pawn = CreateEmpty();
		pawn->transform->Scale({ 7.5, 7.5, 7.5 });
		pawn->transform->Rotate({ 1, 0, 0 }, -90);
		pawn->transform->position = { 0, 0, -1 };
		pawn->AddComponent<MeshFilter>()->mesh = mesh_grid;
		pawn->AddComponent<Renderer>()->material = material_pawn;

		pawn->AddComponent<Pawn>()->x =	-1;
		pawn->GetComponent<Pawn>()->y = -1;

		board->GetComponent<TCPClient>()->pawns.push_back(pawn->GetComponent<Pawn>());
	}
}