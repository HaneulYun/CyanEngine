#include "pch.h"
#include "GameScene.h"

#include "pch.h"
#include "Scene.h"

void GameScene::BuildObjects()
{
	ComPtr<ID3D12Device> _device = rendererManager->device.Get();

	CubeMeshIlluminated* pCubeMesh = new CubeMeshIlluminated(_device.Get(), rendererManager->commandList.Get(), 20.0f, 20.0f, 20.0f);

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	{
		GameObject* Cube = new GameObject;

		MeshFilter* meshFilter = Cube->AddComponent<MeshFilter>();
		meshFilter->mesh = pCubeMesh;

		Renderer* renderer = Cube->AddComponent<Renderer>();
		renderer->material = defaultMaterial;

		RotatingBehavior* rotatingBehavior = Cube->AddComponent<RotatingBehavior>();
		rotatingBehavior->pos = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
		rotatingBehavior->speedRotating = 45.0f;

		gameObjects.push_back(Cube);
	}
}