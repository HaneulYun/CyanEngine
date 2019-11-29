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

		{
			CRawFormatImage* pRawFormatImage = new CRawFormatImage(L"Texture/ObjectsMap.raw", 257, 257, true);
			Terrain* _terrain = terrain->GetComponent<Terrain>();
			int nTerrainWidth = int(_terrain->GetWidth());
			int nTerrainLength = int(_terrain->GetLength());
			XMFLOAT3 xmf3TerrainScale = _terrain->GetScale();

			CTexturedRectMesh* mesh1 = new CTexturedRectMesh(20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
			CTexturedRectMesh* mesh2 = new CTexturedRectMesh(20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
			CTexturedRectMesh* mesh3 = new CTexturedRectMesh(20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
			CTexturedRectMesh* mesh4 = new CTexturedRectMesh(20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
			CTexturedRectMesh* mesh5 = new CTexturedRectMesh(20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
			CTexturedRectMesh* mesh6 = new CTexturedRectMesh(20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
			CTexturedRectMesh* mesh7 = new CTexturedRectMesh(20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
			Material* material01 = new Material(L"Texture/Grass01.dds", ShaderMode::Billboard);
			Material* material02 = new Material(L"Texture/Grass02.dds", ShaderMode::Billboard);
			Material* material03 = new Material(L"Texture/Flower01.dds", ShaderMode::Billboard);
			Material* material04 = new Material(L"Texture/Flower02.dds", ShaderMode::Billboard);
			Material* material05 = new Material(L"Texture/Tree01.dds", ShaderMode::Billboard);
			Material* material06 = new Material(L"Texture/Tree02.dds", ShaderMode::Billboard);
			Material* material07 = new Material(L"Texture/Tree03.dds", ShaderMode::Billboard);

			float xPosition;
			float zPosition;
			for (int nObjects = 0, z = 2; z <= 254; z++)
			{
				for (int x = 2; x <= 254; x++)
				{
					BYTE nPixel = pRawFormatImage->GetRawImagePixel(x, z);
			
					if (!nPixel)
						continue;
			
					GameObject * test = CreateEmpty();

					xPosition = x * xmf3TerrainScale.x;
					zPosition = z * xmf3TerrainScale.z;
			
					switch (nPixel)
					{
					case 102:
						test->AddComponent<MeshFilter>()->mesh = mesh1;
						test->AddComponent<Renderer>()->material = material01;
						test->GetComponent<Renderer>()->material->albedo = { 8, 8, 0, 0 };
						break;
					case 128:
						test->AddComponent<MeshFilter>()->mesh = mesh2;
						test->AddComponent<Renderer>()->material = material02;
						test->GetComponent<Renderer>()->material->albedo = { 8, 6, 0, 0 };
						break;
					case 153:
						test->AddComponent<MeshFilter>()->mesh = mesh3;
						test->AddComponent<Renderer>()->material = material03;
						test->GetComponent<Renderer>()->material->albedo = { 8, 16, 0, 0 };
						break;
					case 179:
						test->AddComponent<MeshFilter>()->mesh = mesh4;
						test->AddComponent<Renderer>()->material = material04;
						test->GetComponent<Renderer>()->material->albedo = { 8, 16, 0, 0 };
						break;
					case 204:
						test->AddComponent<MeshFilter>()->mesh = mesh5;
						test->AddComponent<Renderer>()->material = material05;
						test->GetComponent<Renderer>()->material->albedo = { 24, 36, 0, 0 };
						break;
					case 225:
						test->AddComponent<MeshFilter>()->mesh = mesh6;
						test->AddComponent<Renderer>()->material = material06;
						test->GetComponent<Renderer>()->material->albedo = { 24, 36, 0, 0 };
						break;
					case 255:
						test->AddComponent<MeshFilter>()->mesh = mesh7;
						test->AddComponent<Renderer>()->material = material07;
						test->GetComponent<Renderer>()->material->albedo = { 16, 36, 0, 0 };
						break;
					default:
						break;
					}
					Vector3 t = test->GetComponent<Transform>()->position = {
								x * xmf3TerrainScale.x - 500,
								terrain->GetComponent<Terrain>()->GetHeight(xPosition, zPosition),
								z * xmf3TerrainScale.z - 500 };
				}
			}
		}
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