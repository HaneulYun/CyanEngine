#include "pch.h"
#include "GameScene.h"

#include "pch.h"
#include "Scene.h"

void GameScene::BuildObjects()
{
	ComPtr<ID3D12Device> _device = rendererManager->device.Get();

	CubeMeshIlluminated* pCubeMesh = new CubeMeshIlluminated(_device.Get(), rendererManager->commandList.Get(), 4.0f, 4.0f, 4.0f);

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();


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