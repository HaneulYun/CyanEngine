#include "pch.h"
#include "GameScene.h"

GameObject* Thread::player;

void GameScene::BuildObjects()
{
	ComPtr<ID3D12Device> _device = rendererManager->device.Get();

	Quad* pQuadMesh = new Quad(_device.Get(), rendererManager->commandList.Get(), 10.0f, 10.0f);
	Circle* pCircleMesh = new Circle(_device.Get(), rendererManager->commandList.Get(), 10, 48);
	CircleLine* pCircleLineMesh = new CircleLine(_device.Get(), rendererManager->commandList.Get(), 25.f);
	TriangleMesh* pTriangleMesh = new TriangleMesh(_device.Get(), rendererManager->commandList.Get());

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	GameObject* object = CreateGameObject();
	object->AddComponent<MeshFilter>();
	object->AddComponent<Renderer>()->material = defaultMaterial;

	GameObject* star = Instantiate(object);
	{
		star->GetComponent<MeshFilter>()->mesh = pCircleMesh;
		star->AddComponent<Star>();
	}

	GameObject* orbit = Instantiate(object);
	{
		orbit->GetComponent<MeshFilter>()->mesh = pCircleLineMesh;
	}

	GameObject* bullet = CreateGameObject(object);
	bullet->GetComponent<MeshFilter>()->mesh = pTriangleMesh;
	bullet->AddComponent<Bullet>();

	//for (int i = 0; i < 1; ++i) {
		GameObject* guardian = Instantiate(object);
		{
			guardian->GetComponent<MeshFilter>()->mesh = pQuadMesh;

			RevolvingBehavior* revolvingBehavior = guardian->AddComponent<RevolvingBehavior>();
			revolvingBehavior->target = star;
			revolvingBehavior->radius = 25.f;
			revolvingBehavior->speedRotating = 60.0f;
			revolvingBehavior->angle = 120.0f;// *i;

			guardian->AddComponent<StarGuadian>();
			guardian->GetComponent<StarGuadian>()->bullet = bullet;
		}
	//}

	GameObject* enemy = CreateGameObject(object);
	{
		enemy->GetComponent<MeshFilter>()->mesh = pQuadMesh;
		enemy->AddComponent<MovingBehavior>()->target = star->GetComponent<Transform>();
	}
	
	GameObject* spawner = AddGameObject();
	{
		spawner->AddComponent<Spawner>()->enemy = enemy;
	}

	GameObject* Recvthread = AddGameObject(); {
		Recvthread->AddComponent<Thread>()->player = guardian;
		Recvthread->AddComponent<Thread>()->severip = "192.168.20.58";
	}
}