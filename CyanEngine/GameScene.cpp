#include "pch.h"
#include "GameScene.h"

Scene* Scene::scene{ nullptr };

void GameScene::BuildObjects()
{
	scene = this;

	CubeMeshIlluminated* pCubeMesh = new CubeMeshIlluminated();

	{
		{
			GameObject* test = CreateEmpty();
			test->AddComponent<MeshFilter>()->mesh = new CTexturedRectMesh(20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
			test->AddComponent<Renderer>()->material = new Material(L"Texture/SkyBox_Front_0.dds", ShaderMode::Skybox);
		}
		{
			GameObject* test = CreateEmpty();
			test->AddComponent<MeshFilter>()->mesh = new CTexturedRectMesh(20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -10.0f);
			test->AddComponent<Renderer>()->material = new Material(L"Texture/SkyBox_Back_0.dds", ShaderMode::Skybox);
		}
		{
			GameObject* test = CreateEmpty();
			test->AddComponent<MeshFilter>()->mesh = new CTexturedRectMesh(0.0f, 20.0f, 20.0f, -10.0f, 0.0f, 0.0f);
			test->AddComponent<Renderer>()->material = new Material(L"Texture/SkyBox_Left_0.dds", ShaderMode::Skybox);
		}
		{
			GameObject* test = CreateEmpty();
			test->AddComponent<MeshFilter>()->mesh = new CTexturedRectMesh(0.0f, 20.0f, 20.0f, +10.0f, 0.0f, 0.0f);
			test->AddComponent<Renderer>()->material = new Material(L"Texture/SkyBox_Right_0.dds", ShaderMode::Skybox);
		}
		{
			GameObject* test = CreateEmpty();
			test->AddComponent<MeshFilter>()->mesh = new CTexturedRectMesh(20.0f, 0.0f, 20.0f, 0.0f, +10.0f, 0.0f);
			test->AddComponent<Renderer>()->material = new Material(L"Texture/SkyBox_Top_0.dds", ShaderMode::Skybox);
		}
		{
			GameObject* test = CreateEmpty();
			test->AddComponent<MeshFilter>()->mesh = new CTexturedRectMesh(20.0f, 0.0f, 20.0f, 0.0f, -10.0f, 0.0f);
			test->AddComponent<Renderer>()->material = new Material(L"Texture/SkyBox_Bottom_0.dds", ShaderMode::Skybox);
		}
	}

	{
		{
			GameObject* test = CreateEmpty();
			test->GetComponent<Transform>()->position = { -25, 100, 10 };
			test->AddComponent<MeshFilter>()->mesh = new CCubeMeshTextured(4.0f, 4.0f, 4.0f);;
			test->AddComponent<Renderer>()->material = new Material(L"Texture/Lava(Diffuse).dds");
			test->AddComponent<RotatingBehavior>();
		}
		{
			GameObject* test = CreateEmpty();
			test->GetComponent<Transform>()->position = { -15, 100, 10 };
			test->AddComponent<MeshFilter>()->mesh = new CCubeMeshTextured(4.0f, 4.0f, 4.0f);;
			test->AddComponent<Renderer>()->material = new Material(L"Texture/Stone01.dds");
			test->AddComponent<RotatingBehavior>();
		}
		{
			GameObject* test = CreateEmpty();
			test->GetComponent<Transform>()->position = { -5, 100, 10 };
			test->AddComponent<MeshFilter>()->mesh = new CCubeMeshTextured(4.0f, 4.0f, 4.0f);;
			test->AddComponent<Renderer>()->material = new Material(L"Texture/Metal01.dds");
			test->AddComponent<RotatingBehavior>();
		}
		{
			GameObject* test = CreateEmpty();
			test->GetComponent<Transform>()->position = { 5, 100, 10 };
			test->AddComponent<MeshFilter>()->mesh = new CCubeMeshTextured(4.0f, 4.0f, 4.0f);;
			test->AddComponent<Renderer>()->material = new Material(L"Texture/Metal02.dds");
			test->AddComponent<RotatingBehavior>();
		}
		{
			GameObject* test = CreateEmpty();
			test->GetComponent<Transform>()->position = { 15, 100, 10 };
			test->AddComponent<MeshFilter>()->mesh = new CCubeMeshTextured(4.0f, 4.0f, 4.0f);;
			test->AddComponent<Renderer>()->material = new Material(L"Texture/Rock01.dds");
			test->AddComponent<RotatingBehavior>();
		}
		{
			GameObject* test = CreateEmpty();
			test->GetComponent<Transform>()->position = { 25, 100, 10 };
			test->AddComponent<MeshFilter>()->mesh = new CCubeMeshTextured(4.0f, 4.0f, 4.0f);;
			test->AddComponent<Renderer>()->material = new Material(L"Texture/Lava(Emissive).dds");
			test->AddComponent<RotatingBehavior>();
		}
	}

	GameObject* terrain = CreateEmpty();
	{
		XMFLOAT3 scale = { 1000.f / 257.f, 200.f / 257.f, 1000.f / 257.f };
		terrain->GetComponent<Transform>()->position = { -500, 0, -500 };
		terrain->AddComponent<Terrain>()->LoadTerrain(L"heightMap.raw", 257, 257, 257, 257, scale, { 1, 1, 1, 1 });
		terrain->AddComponent<Renderer>()->material = new Material(nullptr, ShaderMode::Terrain);
	}

	GameObject* player = CreateEmpty();
	{
		player->GetComponent<Transform>()->position = { 0, 100, 0 };
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

		child->children[27]->AddComponent<RotatingBehavior>()->speedRotating = 360;
		child->children[7]->AddComponent<RotatingBehavior>()->speedRotating = -720;

		player->AddComponent<Controller>();
	}
}