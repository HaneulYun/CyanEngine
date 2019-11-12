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



	GameObject* gameObject = new GameObject;
	{
		gameObject->transform->position = XMFLOAT3{ 0.0f, 0.0f, 0.0f };

		MeshFilter* meshFilter = gameObject->AddComponent<MeshFilter>();
		meshFilter->mesh = pCubeMesh;

		Renderer* renderer = gameObject->AddComponent<Renderer>();
		renderer->material = defaultMaterial;

		RotatingBehavior* rotatingBehavior = gameObject->AddComponent<RotatingBehavior>();
		rotatingBehavior->speedRotating = 0.0f;
	}


	int xObjects = 15, yObjects = 15, zObjects = 15, i = 0;
	for (int x = -xObjects; x <= xObjects; x++)
		for (int y = -yObjects; y <= yObjects; y++)
			for (int z = -zObjects; z <= zObjects; z++)
			{
				GameObject* instance = Object::Instantiate(gameObject);

				instance->transform->position = XMFLOAT3{ 10.0f * x, 10.0f * y, 10.0f * z };

				RotatingBehavior* rotatingBehavior = instance->AddComponent<RotatingBehavior>();
				rotatingBehavior->speedRotating = 10.0f * (i++ % 10);

				gameObjects.push_back(instance);
			}
}