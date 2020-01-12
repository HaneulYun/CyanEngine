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
		camera->GenerateProjectionMatrix(0.3f, 10000.0f, float(CyanWindow::m_nWndClientWidth) / float(CyanWindow::m_nWndClientHeight), 90.0f);
		camera->GenerateViewMatrix(XMFLOAT3(0.0f, 10.0f, -15.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		camera->main = camera;
	}

	//GameObject* obj = CreateEmpty();
	//{
	//	obj->GetComponent<Transform>()->position = { 0, 0, 0 };
	//	obj->AddComponent<MeshFilter>();
	//
	//	GameObject* child = ModelManager::Instance()->LoadGeometryFromFile("Model/Apache.bin");
	//	child->GetComponent<Transform>()->Scale({ 0.2f, 0.2f, 0.2f });
	//	obj->AddChild(child);
	//
	//	child->children[27]->AddComponent<RotatingBehavior>()->speedRotating = 180;
	//	child->children[7]->AddComponent<RotatingBehavior>()->speedRotating = -360;
	//
	//	obj->AddComponent<Controller>()->gameObject = obj;
	//}

	GameObject* player = CreateEmpty();
	{
		player->GetComponent<Transform>()->Scale({ 0.1f, 0.1f, 0.1f });
		//player->GetComponent<Transform>()->Rotate(XMFLOAT3{ 0.f,1.f,0.f }, 180);
		player->GetComponent<Transform>()->position = { 0.f, 0.f, 0.f };
		player->AddComponent<FBX_TEST>();
		
		//player->AddComponent<Controller>()->gameObject = player;

		player->AddComponent<RotatingBehavior>()->speedRotating = 60.f;
	}
}