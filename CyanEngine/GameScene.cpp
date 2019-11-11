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

	int xObjects = 15, yObjects = 15, zObjects = 15, i = 0;
	for (int x = -xObjects; x <= xObjects; x++)
		for (int y = -yObjects; y <= yObjects; y++)
			for (int z = -zObjects; z <= zObjects; z++)
			{
				{
					GameObject* instance = new GameObject;
					instance->transform->position = XMFLOAT3{ 50.0f * x, 50.0f * y, 50.0f * z };

					MeshFilter* meshFilter = instance->AddComponent<MeshFilter>();
					meshFilter->mesh = pCubeMesh;

					Renderer* renderer = instance->AddComponent<Renderer>();
					renderer->material = defaultMaterial;

					RotatingBehavior* rotatingBehavior = instance->AddComponent<RotatingBehavior>();
					rotatingBehavior->speedRotating = 10.0f * (i++ % 10);

					gameObjects.push_back(instance);
				}
			}
}