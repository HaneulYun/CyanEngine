#include "pch.h"
#include "GameScene.h"

void GameScene::BuildObjects()
{
	ComPtr<ID3D12Device> _device = rendererManager->device.Get();

	Quad* pQuadMesh = new Quad(_device.Get(), rendererManager->commandList.Get(), 5.0f, 5.0f);

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	GameObject* Star = AddGameObject();
	{
		MeshFilter* meshFilter = Star->AddComponent<MeshFilter>();
		meshFilter->mesh = pQuadMesh;

		Renderer* renderer = Star->AddComponent<Renderer>();
		renderer->material = defaultMaterial;
	}

	GameObject* Quad = AddGameObject();
	{
		MeshFilter* meshFilter = Quad->AddComponent<MeshFilter>();
		meshFilter->mesh = pQuadMesh;

		Renderer* renderer = Quad->AddComponent<Renderer>();
		renderer->material = defaultMaterial;

		RotatingBehavior* rotatingBehavior = Quad->AddComponent<RotatingBehavior>();
		rotatingBehavior->pos = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
		rotatingBehavior->speedRotating = 45.0f;

		RevolvingBehavior* revolvingBehavior = Quad->AddComponent<RevolvingBehavior>();
		revolvingBehavior->target = Star;
		revolvingBehavior->speedRotating = 60.0f;
	}
}