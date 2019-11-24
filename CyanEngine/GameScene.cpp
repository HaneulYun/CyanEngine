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

	GameObject* cube = CreateEmpty();
	{
		cube->AddComponent<MeshFilter>()->mesh = pCubeMesh;
		cube->AddComponent<Renderer>()->material = defaultMaterial;
	}


	GameObject* sceneManager = CreateEmpty();
	{
		sceneManager->AddComponent<SceneManager>()->gameObject = cube;
	}
}