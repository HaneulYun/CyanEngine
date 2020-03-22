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

	rendererManager->materials["board"] = std::make_unique<Material>();
	Material* material_bricks0 = rendererManager->materials["board"].get();

	{
		material_bricks0->Name = "board";
		material_bricks0->MatCBIndex = 0;
		material_bricks0->DiffuseSrvHeapIndex = 0;
		material_bricks0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		material_bricks0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
		material_bricks0->Roughness = 0.1f;
	}


	//*** Game Object ***//

	GameObject* mainCamera = CreateEmpty();
	{
		Camera* camera = mainCamera->AddComponent<Camera>();
		camera->SetViewport(0, 0, CyanFW::Instance()->GetWidth(), CyanFW::Instance()->GetHeight(), 0.0f, 1.0f);
		camera->SetScissorRect(0, 0, CyanFW::Instance()->GetWidth(), CyanFW::Instance()->GetHeight());
		camera->GenerateProjectionMatrix(0.3f, 10000.0f, CyanFW::Instance()->GetAspectRatio() , XMConvertToDegrees(0.25f * PI));
		//camera->GenerateOrthoMatrix(800, 600, 0, 100);
		camera->GenerateViewMatrix(XMFLOAT3(0.0f, 0.0f, -15.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		camera->main = camera;

		mainCamera->AddComponent<CameraController>();
	}

	GameObject* player = CreateEmpty();
	{
		player->AddComponent<Controller>()->gameObject = player;
	}

	{
		GameObject* grid = CreateEmpty();

		grid->GetComponent<Transform>()->Rotate({ 1, 0, 0 }, -90);
		grid->AddComponent<MeshFilter>()->mesh = mesh_grid;
		grid->AddComponent<Renderer>()->material = material_defaultMaterial;
	}
}