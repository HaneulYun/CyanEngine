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
		GameObject* test = CreateEmpty();
		test->GetComponent<Transform>()->position = { 0, 55, 0 };
		test->AddComponent<MeshFilter>()->mesh = new CTexturedRectMesh(1000.0f, 0.0f, 1000.0f, 0.0f, -0.01, 0.0f);
		test->AddComponent<Renderer>()->material = new Material(L"Texture/Water.dds");
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
		terrain->AddComponent<Renderer>()->material = new Material(nullptr, ShaderMode::Terrain);
#ifdef _WITH_TERRAIN_PARTITION
		terrain->AddComponent<Terrain>()->LoadTerrain(L"heightMap.raw", 257, 257, 9, 9, scale, { 1, 1, 1, 1 });
#else
		terrain->AddComponent<Terrain>()->LoadTerrain(L"heightMap.raw", 257, 257, 9, 9, scale, { 1, 1, 1, 1 });
#endif

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
			Mesh* mesh;
			Material* material{ nullptr };
			Vector3 vec;
			static int t = 0;
			for (int nObjects = 0, z = 2; z <= 254; z++)
			{
				for (int x = 2; x <= 254; x++)
				{
					BYTE nPixel = pRawFormatImage->GetRawImagePixel(x, z);

					xPosition = x * xmf3TerrainScale.x;
					zPosition = z * xmf3TerrainScale.z;

					switch (nPixel)
					{
					case 102:
						mesh = mesh1;
						material = material01;
						vec = { 8, 8, 0 };
						break;
					case 128:
						mesh = mesh2;
						material = material02;
						vec = { 8, 6, 0 };
						break;
					case 153:
						mesh = mesh3;
						material = material03;
						vec = { 8, 16, 0 };
						break;
					case 179:
						mesh = mesh4;
						material = material04;
						vec = { 8, 16, 0 };
						break;
					case 204:
						mesh = mesh5;
						material = material05;
						vec = { 24, 36, 0 };
						break;
					case 225:
						mesh = mesh6;
						material = material06;
						vec = { 24, 36, 0 };
						break;
					case 255:
						mesh = mesh7;
						material = material07;
						vec = { 16, 36, 0 };
						break;
					default:
						continue;
						break;
					}
					++t;
					GameObject* test = CreateEmpty(false);
					test->AddComponent<MeshFilter>()->mesh = mesh;
					test->AddComponent<Renderer>()->material = material;
					test->GetComponent<Renderer>()->material->albedo = { vec.x, vec.y, vec.z, 1 };
					
					test->GetComponent<Transform>()->position = {
								x * xmf3TerrainScale.x - 500,
								terrain->GetComponent<Terrain>()->GetHeight(xPosition, zPosition) + vec.y / 2,
								z * xmf3TerrainScale.z - 500 };
					test->Start();
				}
			}
			char str[10];
			sprintf(str, "%d\n", t);
			Debug::Log(str);
		}
	}

	GameObject* player = CreateEmpty();
	{
		player->GetComponent<Transform>()->position = { 0, 100, 0 };
		player->AddComponent<MeshFilter>();

		Camera* camera = player->AddComponent<Camera>();
		camera->SetViewport(0, 0, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight, 0.0f, 1.0f);
		camera->SetScissorRect(0, 0, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight);
		camera->GenerateProjectionMatrix(0.3f, 10000.0f, float(CyanWindow::m_nWndClientWidth) / float(CyanWindow::m_nWndClientHeight), 90.0f);
		camera->GenerateViewMatrix(XMFLOAT3(0.41f, 10.0f, -15.0f), XMFLOAT3(0.41f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		camera->main = camera;

		GameObject* child = ModelManager::Instance()->LoadGeometryFromFile("Model/Apache.bin");
		child->GetComponent<Transform>()->Scale({ 0.2f, 0.2f, 0.2f });
		player->AddChild(child);

		child->children[27]->AddComponent<RotatingBehavior>()->speedRotating = 180;
		child->children[7]->AddComponent<RotatingBehavior>()->speedRotating = -360;

		player->AddComponent<Controller>()->gameObject = player                                                                                                                                                                  ;
	}
}