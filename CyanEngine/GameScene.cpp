#include "pch.h"
#include "GameScene.h"

Scene* Scene::scene{ nullptr };

void GameScene::BuildObjects()
{
	scene = this;

	CubeMeshIlluminated* pCubeMesh = new CubeMeshIlluminated();

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	GameObject* terrain = CreateEmpty();
	{
		XMFLOAT3 scale = { 1000.f / 257.f, 100.f / 257.f, 1000.f / 257.f };
		terrain->GetComponent<Transform>()->position = { -500, -500, 0 };
		terrain->AddComponent<Terrain>()->LoadTerrain(L"heightMap.raw", 257, 257, 257, 257, scale, { 1, 1, 1, 1 });
		terrain->AddComponent<Renderer>()->material = defaultMaterial;
	}

	GameObject* player = CreateEmpty();
	{
		player->AddComponent<MeshFilter>();

		Camera* camera = player->AddComponent<Camera>();
		camera->SetViewport(0, 0, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight, 0.0f, 1.0f);
		camera->SetScissorRect(0, 0, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight);
		camera->GenerateProjectionMatrix(0.3f, 1000.0f, float(CyanWindow::m_nWndClientWidth) / float(CyanWindow::m_nWndClientHeight), 90.0f);
		camera->GenerateViewMatrix(XMFLOAT3(0.0f, 10.0f, -15.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		camera->main = camera;

		GameObject* child = ModelManager::Instance()->LoadGeometryFromFile("Model/Apache.bin");
		child->GetComponent<Transform>()->Scale({ 0.2f, 0.2f, 0.2f });
		player->AddChild(child);
		player->AddComponent<RotatingBehavior>()->speedRotating = 0;// Random::Range(-90.f, 90.f);
		
		child->children[27]->AddComponent<RotatingBehavior>()->speedRotating = 360;
		//child->children[6]->AddComponent<RotatingBehavior>()->speedRotating = 360;
		//child->children[6]->GetComponent<RotatingBehavior>()->axis = { 0.3, 0, 0.3 };
		child->children[7]->AddComponent<RotatingBehavior>()->speedRotating = -720;


		player->AddComponent<Controller>();
	}
}