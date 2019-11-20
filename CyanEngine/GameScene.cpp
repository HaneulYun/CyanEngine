#include "pch.h"
#include "GameScene.h"

void GameScene::BuildObjects()
{
	CubeMeshIlluminated* pCubeMesh = new CubeMeshIlluminated(4.0f, 4.0f, 4.0f);

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	GameObject* terrain = AddGameObject();
	{
		XMFLOAT3 scale(5.0f, 0.5f, 5.0f);
		XMFLOAT4 color(0.0f, 0.2f, 0.0f, 0.0f);

		terrain->GetComponent<Transform>()->position.x = -64 * 5 * 2;
		terrain->GetComponent<Transform>()->position.z = -64 * 5 * 2;
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

	float interval = 150.f;
	int xObjects = 0, yObjects = 0, zObjects = 0, i = 0;
	for (int x = -xObjects; x <= xObjects; x++)
		for (int y = -yObjects; y <= yObjects; y++)
			for (int z = -zObjects; z <= zObjects; z++)
			{
				GameObject* model = AddGameObject();

				model->transform->position = Vector3{ interval * x, interval * y, interval * z };
				model->AddChild(ModelManager::Instance()->LoadGeometryFromFile("Model/Apache.bin"));
				model->AddComponent<RotatingBehavior>();
			}

	//GameObject* _sceneManager = AddGameObject();
	//{
	//	SceneManager* sceneManager = _sceneManager->AddComponent<SceneManager>();
	//	sceneManager->gameObject = cube;
	//}
}