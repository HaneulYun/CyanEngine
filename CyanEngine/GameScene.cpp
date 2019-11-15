#include "pch.h"
#include "GameScene.h"

void GameScene::BuildObjects()
{
	ComPtr<ID3D12Device> _device = rendererManager->device.Get();

	Quad* pQuadMesh = new Quad(_device.Get(), rendererManager->commandList.Get(), 5.0f, 5.0f);
	CircleLine* pCircleLineMesh = new CircleLine(_device.Get(), rendererManager->commandList.Get(), 10.f);
	TriangleMesh* pTriangleMesh = new TriangleMesh(_device.Get(), rendererManager->commandList.Get());

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	GameObject* object = CreateGameObject();
	object->AddComponent<MeshFilter>();
	object->AddComponent<Renderer>()->material = defaultMaterial;

	GameObject* bullet = CreateGameObject(object);
	bullet->GetComponent<MeshFilter>()->mesh = pTriangleMesh;
	bullet->AddComponent<Bullet>();

	GameObject* star = Instantiate(object);
	{
		star->GetComponent<MeshFilter>()->mesh = pQuadMesh;
		star->AddComponent<Star>();
		star->GetComponent<Star>()->bullet = bullet;
	}

	GameObject* orbit = Instantiate(object);
	{
		orbit->GetComponent<MeshFilter>()->mesh = pCircleLineMesh;
	}

	for (int i = 0; i < 3; ++i) {
		GameObject* guardian = Instantiate(object);
		{
			guardian->GetComponent<MeshFilter>()->mesh = pQuadMesh;

			RevolvingBehavior* revolvingBehavior = guardian->AddComponent<RevolvingBehavior>();
			revolvingBehavior->target = star;
			revolvingBehavior->radius = 10.f;
			revolvingBehavior->speedRotating = 60.0f;
			revolvingBehavior->angle = 120.0f * i;
		}
	}

	GameObject* enemy = CreateGameObject(object);
	{
		enemy->GetComponent<MeshFilter>()->mesh = pQuadMesh;
		enemy->AddComponent<MovingBehavior>()->target = star->GetComponent<Transform>();
	}
	
	GameObject* spawner = AddGameObject();
	{
		spawner->AddComponent<Spawner>()->enemy = enemy;
	}


	
}