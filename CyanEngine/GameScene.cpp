#include "pch.h"
#include "GameScene.h"

Scene* Scene::scene{ nullptr };

void GameScene::BuildObjects()
{
	scene = this;

	CubeMeshIlluminated* pCubeMesh = new CubeMeshIlluminated(14.0f, 14.0f, 14.0f);

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	//GameObject* terrain = CreateEmpty();
	//{
	//	XMFLOAT3 scale(5.0f, 0.5f, 5.0f);
	//	XMFLOAT4 color(0.0f, 0.2f, 0.0f, 0.0f);
	//
	//	terrain->GetComponent<Transform>()->position.x = -64 * 5 * 2;
	//	terrain->GetComponent<Transform>()->position.z = -64 * 5 * 2;
	//	terrain->GetComponent<Transform>()->position.y = -100.f;
	//	terrain->AddComponent<Terrain>()->LoadTerrain(L"heightMap.raw", 257, 257, 257, 257, scale, color);
	//	terrain->AddComponent<Renderer>()->material = defaultMaterial;
	//	terrain->GetComponent<Renderer>()->material->albedo = XMFLOAT4{ 71.f / 255.f, 102.f / 255.f, 0.f, 1 };
	//}
	//
	//GameObject* cube = CreateEmpty();
	//{
	//	cube->AddComponent<MeshFilter>()->mesh = pCubeMesh;
	//	cube->AddComponent<Renderer>()->material = defaultMaterial;
	//	cube->AddComponent<RotatingBehavior>();
	//	cube->AddComponent<MovingBehavior>();
	//}

	{
		GameObject* model = CreateEmpty();
		model->transform->position = Vector3{ 0, 0, 400 };
		model->AddComponent<MeshFilter>()->mesh = pCubeMesh;
		model->AddComponent<Renderer>()->material = defaultMaterial;

		GameObject* child = ModelManager::Instance()->LoadGeometryFromFile("Model/Apache.bin");
		child->parent = model;

		child->transform->position = Vector3{ 0, 0, 400 };
		child->AddComponent<MeshFilter>()->mesh = pCubeMesh;
		child->AddComponent<Renderer>()->material = defaultMaterial;

		model->AddChild(child);
		model->AddComponent<RotatingBehavior>();
	}

	{
		GameObject* model = CreateEmpty();
		model->transform->position = Vector3{ -400, 0, 0 };
		model->AddComponent<MeshFilter>()->mesh = pCubeMesh;
		model->AddComponent<Renderer>()->material = defaultMaterial;

		GameObject* child = ModelManager::Instance()->LoadGeometryFromFile("Model/Apache.bin");
		
		child->parent = model;
		child->transform->position = Vector3{ 0, 0, 400 };
		child->AddComponent<MeshFilter>()->mesh = pCubeMesh;
		child->AddComponent<Renderer>()->material = defaultMaterial;
		
		model->AddChild(child);
		model->AddComponent<RotatingBehavior>();
	}

	{
		GameObject* model = CreateEmpty();
		model->transform->position = Vector3{ 400, 0, 0 };
		model->AddComponent<MeshFilter>()->mesh = pCubeMesh;
		model->AddComponent<Renderer>()->material = defaultMaterial;

		GameObject* child = ModelManager::Instance()->LoadGeometryFromFile("Model/Apache.bin");
		child->parent = model;
		model->AddChild(child);
		model->AddComponent<RotatingBehavior>();
	}

	//float interval = 150.f;
	//int xObjects = 1, yObjects = 0, zObjects = 1, i = 0;
	//for (int x = -xObjects; x <= xObjects; x++)
	//	for (int y = -yObjects; y <= yObjects; y++)
	//		for (int z = -zObjects; z <= zObjects; z++)
	//		{
	//			GameObject* model = CreateEmpty();
	//			model->AddComponent<MeshFilter>()->mesh = pCubeMesh;
	//			model->AddComponent<Renderer>()->material = defaultMaterial;
	//
	//			model->transform->position = Vector3{ interval * x, interval * y, interval * z };
	//
	//			GameObject* child = ModelManager::Instance()->LoadGeometryFromFile("Model/Apache.bin");
	//			child->parent = model;
	//			model->AddChild(child);
	//			model->AddComponent<RotatingBehavior>();
	//		}

	//GameObject* _sceneManager = AddGameObject();
	//{
	//	SceneManager* sceneManager = _sceneManager->AddComponent<SceneManager>();
	//	sceneManager->gameObject = cube;
	//}
}