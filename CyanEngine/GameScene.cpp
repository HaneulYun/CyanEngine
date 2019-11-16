#include "pch.h"
#include "GameScene.h"

Material* SceneManager::materials[16];

void GameScene::BuildObjects()
{
	CubeMeshIlluminated* pCubeMesh = new CubeMeshIlluminated(4.0f, 4.0f, 4.0f);

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	for (auto& material : SceneManager::materials)
	{
		float r = Random::Range(0.0f, 1.0f);
		float g = Random::Range(0.0f, 1.0f);
		float b = Random::Range(0.0f, 1.0f);
		material = new DefaultMaterial;
		material->shader = defaultMaterial->shader;
		material->albedo = XMFLOAT4(r, g, b, 1);
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