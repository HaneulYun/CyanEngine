#include "pch.h"
#include "GameScene.h"

void GameScene::BuildObjects()
{
	ComPtr<ID3D12Device> _device = rendererManager->device.Get();

	Quad* pQuadMesh = new Quad(_device.Get(), rendererManager->commandList.Get(), 5.0f, 5.0f);
	CircleLine* pCircleLineMesh = new CircleLine(_device.Get(), rendererManager->commandList.Get(), 10.f);

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	GameObject* Star = AddGameObject();
	{
		MeshFilter* meshFilter = Star->AddComponent<MeshFilter>();
		meshFilter->mesh = pQuadMesh;

		Renderer* renderer = Star->AddComponent<Renderer>();
		renderer->material = defaultMaterial;
	}
	GameObject* Orbit = AddGameObject();
	{
		MeshFilter* meshFilter = Orbit->AddComponent<MeshFilter>();
		meshFilter->mesh = pCircleLineMesh;

		Renderer* renderer = Orbit->AddComponent<Renderer>();
		renderer->material = defaultMaterial;
	}
	for (int i = 0; i < 3; ++i) {
		GameObject* Guardian = AddGameObject();
		{
			MeshFilter* meshFilter = Guardian->AddComponent<MeshFilter>();
			meshFilter->mesh = pQuadMesh;

			Renderer* renderer = Guardian->AddComponent<Renderer>();
			renderer->material = defaultMaterial;

			/*RotatingBehavior* rotatingBehavior = Guardian->AddComponent<RotatingBehavior>();
			rotatingBehavior->pos = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
			rotatingBehavior->speedRotating = 45.0f;*/

			RevolvingBehavior* revolvingBehavior = Guardian->AddComponent<RevolvingBehavior>();
			revolvingBehavior->target = Star;
			revolvingBehavior->radius = 10.f;
			revolvingBehavior->speedRotating = 60.0f;
			revolvingBehavior->angle = 120.0f * i;
		}
	}
}