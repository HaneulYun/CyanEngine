#include "pch.h"
#include "SampleScene.h"

Scene* Scene::scene{ nullptr };

void SampleScene::BuildObjects()
{
	scene = this;

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

	GameObject* cube = CreateEmpty();
	{
		cube->AddComponent<MeshFilter>()->mesh = new CubeMeshDiffused();
		cube->AddComponent<Renderer>()->material = new DefaultMaterial();
	}
}