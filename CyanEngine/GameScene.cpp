#include "pch.h"
#include "GameScene.h"

#include "pch.h"
#include "Scene.h"

void GameScene::BuildObjects()
{
	ComPtr<ID3D12Device> _device = rendererManager->device.Get();

	CubeMeshIlluminated* pCubeMesh = new CubeMeshIlluminated(_device.Get(), rendererManager->commandList.Get(), 1.0f, 1.0f, 1.0f);

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();



	GameObject* gameObject = CreateGameObject();
	{
		MeshFilter* meshFilter = gameObject->AddComponent<MeshFilter>();
		meshFilter->mesh = pCubeMesh;

		Renderer* renderer = gameObject->AddComponent<Renderer>();
		renderer->material = defaultMaterial;
	}

	GameObject* _sceneManager = AddGameObject();
	{
		SceneManager* sceneManager = _sceneManager->AddComponent<SceneManager>();
		sceneManager->gameObject = gameObject;
	} 

	AddGameObject();
}