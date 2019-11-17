#include "pch.h"
#include "GameScene.h"

void GameScene::BuildObjects()
{
	CubeMeshIlluminated* pCubeMesh = new CubeMeshIlluminated(4.0f, 4.0f, 4.0f);

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	GameObject* terrain = AddGameObject();
	{
		XMFLOAT3 scale(2.0f, 0.5f, 2.0f);
		XMFLOAT4 color(0.0f, 0.2f, 0.0f, 0.0f);

		terrain->GetComponent<Transform>()->position.x = -128*2;
		terrain->GetComponent<Transform>()->position.y = -100.f;
		terrain->AddComponent<Terrain>()->LoadTerrain(L"heightMap.raw", 257, 257, 257, 257, scale, color);
		terrain->AddComponent<Renderer>()->material = defaultMaterial;
		terrain->GetComponent<Renderer>()->material->albedo = XMFLOAT4{ 71.f / 255.f, 102.f / 255.f, 0.f, 1 };
	}

	GameObject* cube = CreateGameObject();
	{
		cube->AddComponent<MeshFilter>()->mesh = pCubeMesh;
		cube->AddComponent<Renderer>()->material = defaultMaterial;
		cube->AddComponent<RotatingBehavior>();
		cube->AddComponent<MovingBehavior>();
	}

	GameObject* model = AddGameObject();
	model->LoadGeometryFromFile("Model/Apache.bin");
	
	GameObject* _sceneManager = AddGameObject();
	{
		SceneManager* sceneManager = _sceneManager->AddComponent<SceneManager>();
		sceneManager->gameObject = cube;
	}
}