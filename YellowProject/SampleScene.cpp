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

	rendererManager->materials["bricks0"] = std::make_unique<Material>();
	rendererManager->materials["stone0"] = std::make_unique<Material>();
	rendererManager->materials["tile0"] = std::make_unique<Material>();
	rendererManager->materials["skullMat"] = std::make_unique<Material>();
	Material* material_bricks0 = rendererManager->materials["bricks0"].get();
	Material* material_stone0 = rendererManager->materials["stone0"].get();
	Material* material_tile0 = rendererManager->materials["tile0"].get();
	Material* material_skullMat = rendererManager->materials["skullMat"].get();

	{
		material_bricks0->Name = "bricks0";
		material_bricks0->MatCBIndex = 0;
		material_bricks0->DiffuseSrvHeapIndex = 0;
		material_bricks0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		material_bricks0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
		material_bricks0->Roughness = 0.1f;

		material_stone0->Name = "stone0";
		material_stone0->MatCBIndex = 1;
		material_stone0->DiffuseSrvHeapIndex = 1;
		material_stone0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		material_stone0->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
		material_stone0->Roughness = 0.3f;

		material_tile0->Name = "tile0";
		material_tile0->MatCBIndex = 2;
		material_tile0->DiffuseSrvHeapIndex = 2;
		material_tile0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		material_tile0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
		material_tile0->Roughness = 0.2f;

		material_skullMat->Name = "skullMat";
		material_skullMat->MatCBIndex = 3;
		material_skullMat->DiffuseSrvHeapIndex = 3;
		material_skullMat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		material_skullMat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05);
		material_skullMat->Roughness = 0.3f;
	}


	//*** Game Object ***//

	GameObject* mainCamera = CreateEmpty();
	{
		Camera* camera = mainCamera->AddComponent<Camera>();
		camera->SetViewport(0, 0, CyanFW::Instance()->GetWidth(), CyanFW::Instance()->GetHeight(), 0.0f, 1.0f);
		camera->SetScissorRect(0, 0, CyanFW::Instance()->GetWidth(), CyanFW::Instance()->GetHeight());
		camera->GenerateProjectionMatrix(0.3f, 10000.0f, CyanFW::Instance()->GetAspectRatio() , XMConvertToDegrees(0.25f * PI));
		camera->GenerateViewMatrix(XMFLOAT3(0.0f, 0.0f, -15.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		camera->main = camera;

		mainCamera->AddComponent<CameraController>();
	}

	GameObject* player = CreateEmpty();
	{
		player->AddComponent<Controller>()->gameObject = player;
	}

	int cnt = 1;
	float offset = 2.0f;
	for (int x = -cnt; x <= cnt; ++x)
		for (int y = -cnt; y <= cnt; ++y)
			for (int z = -cnt; z <= cnt; ++z)
				{
					GameObject* cube = CreateEmpty();
					cube->GetComponent<Transform>()->position = Vector3(x * offset, y * offset + 5, z * offset + 5);
					cube->AddComponent<MeshFilter>()->mesh = mesh_cube;
					cube->AddComponent<Renderer>()->material = material_defaultMaterial;
				}

	{
		GameObject* cube = CreateEmpty();
		cube->GetComponent<Transform>()->position = Vector3(0, 1, 0);
		cube->GetComponent<Transform>()->localScale = Vector3(2, 2, 2);
		cube->AddComponent<MeshFilter>()->mesh = mesh_cube;
		cube->AddComponent<Renderer>()->material = material_defaultMaterial;
	}

	{
		GameObject* grid = CreateEmpty();
		grid->AddComponent<MeshFilter>()->mesh = mesh_grid;
		grid->AddComponent<Renderer>()->material = material_defaultMaterial;
	}

	for (int i = 0; i < 5; ++i)
	{
		GameObject* leftCylRItem = CreateEmpty();
		leftCylRItem->GetComponent<Transform>()->position = Vector3(-5.0f, 1.5f, -10.0f + i * 5.0f);
		leftCylRItem->AddComponent<MeshFilter>()->mesh = mesh_cylinder;
		leftCylRItem->AddComponent<Renderer>()->material = material_bricks0;

		GameObject* rightCylRItem = CreateEmpty();
		rightCylRItem->GetComponent<Transform>()->position = Vector3(5.0f, 1.5f, -10.0f + i * 5.0f);
		rightCylRItem->AddComponent<MeshFilter>()->mesh = mesh_cylinder;
		rightCylRItem->AddComponent<Renderer>()->material = material_bricks0;

		GameObject* leftSphereRItem = CreateEmpty();
		leftSphereRItem->GetComponent<Transform>()->position = Vector3(-5.0f, 3.5f, -10.0f + i * 5.0f);
		leftSphereRItem->AddComponent<MeshFilter>()->mesh = mesh_sphere;
		leftSphereRItem->AddComponent<Renderer>()->material = material_tile0;

		GameObject* rightSphereRItem = CreateEmpty();
		rightSphereRItem->GetComponent<Transform>()->position = Vector3(5.0f, 3.5f, -10.0f + i * 5.0f);
		rightSphereRItem->AddComponent<MeshFilter>()->mesh = mesh_sphere;
		rightSphereRItem->AddComponent<Renderer>()->material = material_skullMat;
	}
}