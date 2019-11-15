#include "pch.h"
#include "GameScene.h"

void GameScene::BuildObjects()
{
	ComPtr<ID3D12Device> _device = rendererManager->device.Get();

	Quad* pQuadMesh = new Quad(_device.Get(), rendererManager->commandList.Get(), 5.0f, 5.0f);
	CircleLine* pCircleLineMesh = new CircleLine(_device.Get(), rendererManager->commandList.Get(), 10.f);

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	GameObject* object = CreateGameObject();
	object->AddComponent<MeshFilter>();
	object->AddComponent<Renderer>()->material = defaultMaterial;

	GameObject* Star = Instantiate(object);
	{
		Star->GetComponent<MeshFilter>()->mesh = pQuadMesh;
	}

	GameObject* Orbit = Instantiate(object);
	{
		Orbit->GetComponent<MeshFilter>()->mesh = pCircleLineMesh;
	}

	for (int i = 0; i < 3; ++i) {
		GameObject* Guardian = Instantiate(object);
		{
			Guardian->GetComponent<MeshFilter>()->mesh = pQuadMesh;

			RevolvingBehavior* revolvingBehavior = Guardian->AddComponent<RevolvingBehavior>();
			revolvingBehavior->target = Star;
			revolvingBehavior->radius = 10.f;
			revolvingBehavior->speedRotating = 60.0f;
			revolvingBehavior->angle = 120.0f * i;
		}
	}

	GameObject* enemy = CreateGameObject(object);
	{
		enemy->GetComponent<MeshFilter>()->mesh = pQuadMesh;
		enemy->AddComponent<MovingBehavior>()->target = Star->GetComponent<Transform>();
	}
	
	GameObject* spawner = AddGameObject();
	{
		spawner->AddComponent<Spawner>()->enemy = enemy;
	}
}