#include "pch.h"
#include "GameScene.h"

Scene* Scene::scene{ nullptr };

void GameScene::BuildObjects()
{
	scene = this;

	GameObject* mainCamera = CreateEmpty();
	{
		Camera* camera = mainCamera->AddComponent<Camera>();
		camera->SetViewport(0, 0, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight, 0.0f, 1.0f);
		camera->SetScissorRect(0, 0, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight);
		camera->GenerateProjectionMatrix(0.3f, 1000.0f, float(CyanWindow::m_nWndClientWidth) / float(CyanWindow::m_nWndClientHeight), 90.0f);
		camera->GenerateViewMatrix(XMFLOAT3(0.0f, 0.0f, -10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		camera->main = camera;
	}

	GameObject* cube = CreateEmptyPrefab();
	{
		cube->GetComponent<Transform>()->position = { 0,0,0 };
		cube->AddComponent<MeshFilter>()->mesh = new CubeMeshIlluminated(1, 1, 0.2);
		cube->AddComponent<Renderer>()->material= new Material();
		cube->AddComponent<RotatingBehavior>();
	}

	GameObject* sceneManager = CreateEmpty();
	{
		sceneManager->AddComponent<SceneManager>()->gameObject = cube;
	}
}