#include "pch.h"
#include "GameScene.h"

Scene* Scene::scene{ nullptr };

void GameScene::BuildObjects()
{
	scene = this;

	CubeMeshIlluminated* pCubeMesh = new CubeMeshIlluminated(14.0f, 14.0f, 14.0f);

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	GameObject* cube = CreateEmpty();
	{
		cube->AddComponent<MeshFilter>()->mesh = pCubeMesh;
		cube->AddComponent<Renderer>()->material = defaultMaterial;
		cube->AddComponent<BoxCollider>()->extents = { 7, 7, 7 };
	}


	GameObject* sceneManager = CreateEmpty();
	{
		sceneManager->AddComponent<SceneManager>()->gameObject = cube;
	}
}