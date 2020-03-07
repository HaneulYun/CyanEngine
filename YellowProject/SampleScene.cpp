#include "pch.h"
#include "SampleScene.h"

Scene* Scene::scene{ nullptr };

void SampleScene::BuildObjects()
{
	scene = this;

	//*** Asset ***//

	Mesh* mesh_cube = new Cube();
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
}