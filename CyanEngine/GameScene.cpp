#include "pch.h"
#include "GameScene.h"

void GameScene::BuildObjects()
{
	CubeMeshIlluminated* pCubeMesh = new CubeMeshIlluminated(4.0f, 4.0f, 4.0f);

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	GameObject* terrain = AddGameObject();
	{
		terrain->GetComponent<Transform>()->position.x = -128*2;
		terrain->GetComponent<Transform>()->position.y = -100.f;
		terrain->AddComponent<Terrain>();
		terrain->AddComponent<Renderer>()->material = defaultMaterial;
		terrain->GetComponent<Renderer>()->material->albedo = XMFLOAT4(71.f / 255.f, 102.f / 255.f, 0.f, 1);
	}

	GameObject* cube = CreateGameObject();
	{
		cube->AddComponent<MeshFilter>()->mesh = pCubeMesh;
		cube->AddComponent<Renderer>()->material = defaultMaterial;
		cube->AddComponent<RotatingBehavior>();
		cube->AddComponent<MovingBehavior>();
	}
	
	GameObject* _sceneManager = AddGameObject();
	{
		SceneManager* sceneManager = _sceneManager->AddComponent<SceneManager>();
		sceneManager->gameObject = cube;
	}
}