#include "pch.h"
#include "GameScene.h"

#include "pch.h"
#include "Scene.h"

void GameScene::BuildObjects()
{
	ComPtr<ID3D12Device> _device = rendererManager->device.Get();

	Quad* pQuadMesh = new Quad(_device.Get(), rendererManager->commandList.Get(), 10.0f, 10.0f);
	//Circle* pCircleMesh = new Circle(_device.Get(), rendererManager->commandList.Get(), 15.0f, 5);

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	{
		GameObject* Quad = AddGameObject();

		MeshFilter* meshFilter = Quad->AddComponent<MeshFilter>();
		meshFilter->mesh = pQuadMesh;

		Renderer* renderer = Quad->AddComponent<Renderer>();
		renderer->material = defaultMaterial;

		RotatingBehavior* rotatingBehavior = Quad->AddComponent<RotatingBehavior>();
		rotatingBehavior->pos = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
		rotatingBehavior->speedRotating = 0.0f;

		gameObjects.push_back(Quad);
	}
}