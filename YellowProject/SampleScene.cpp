#include "pch.h"
#include "SampleScene.h"

Scene* Scene::scene{ nullptr };

void SampleScene::BuildObjects()
{
	scene = this;

	//*** Asset ***//

	Mesh* mesh_cube = new Cube();
	Mesh* mesh_grid = new Plane();
	Mesh* mesh_sphere = new Sphere();
	Mesh* mesh_cylinder = new Cylinder();
	Material* material_defaultMaterial = new DefaultMaterial();


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
		leftCylRItem->AddComponent<Renderer>()->material = material_defaultMaterial;

		GameObject* rightCylRItem = CreateEmpty();
		rightCylRItem->GetComponent<Transform>()->position = Vector3(5.0f, 1.5f, -10.0f + i * 5.0f);
		rightCylRItem->AddComponent<MeshFilter>()->mesh = mesh_cylinder;
		rightCylRItem->AddComponent<Renderer>()->material = material_defaultMaterial;

		GameObject* leftSphereRItem = CreateEmpty();
		leftSphereRItem->GetComponent<Transform>()->position = Vector3(-5.0f, 3.5f, -10.0f + i * 5.0f);
		leftSphereRItem->AddComponent<MeshFilter>()->mesh = mesh_sphere;
		leftSphereRItem->AddComponent<Renderer>()->material = material_defaultMaterial;

		GameObject* rightSphereRItem = CreateEmpty();
		rightSphereRItem->GetComponent<Transform>()->position = Vector3(5.0f, 3.5f, -10.0f + i * 5.0f);
		rightSphereRItem->AddComponent<MeshFilter>()->mesh = mesh_sphere;
		rightSphereRItem->AddComponent<Renderer>()->material = material_defaultMaterial;
	}
}