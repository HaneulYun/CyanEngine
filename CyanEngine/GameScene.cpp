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

	int xObjects = 30, yObjects = 15, zObjects = 30, i = 0;
	for (int x = -xObjects; x <= xObjects; x++)
		for (int y = -yObjects; y <= yObjects; y++)
			for (int z = -zObjects; z <= zObjects; z++)
			{
				{
					GameObject* Cube = new GameObject;

					MeshFilter* meshFilter = Cube->AddComponent<MeshFilter>();
					meshFilter->mesh = pCubeMesh;

					Renderer* renderer = Cube->AddComponent<Renderer>();
					renderer->material = defaultMaterial;

					RotatingBehavior* rotatingBehavior = Cube->AddComponent<RotatingBehavior>();
					rotatingBehavior->pos = XMFLOAT3{ 50.0f * x, 50.0f * y, 50.0f * z };
					rotatingBehavior->speedRotating = 10.0f * (i++ % 10);

					gameObjects.push_back(Cube);
				}
			}
}