#include "pch.h"
#include "GameScene.h"

GameObject* SceneManager::player[3];
GameObject* SceneManager::playerprefab;
GameObject* SceneManager::scenemanager;

void GameScene::BuildObjects()
{
	Quad* pQuadMesh = new Quad(10.0f, 10.0f);
	Circle* pCircleMesh = new Circle(10, 48);
	CircleLine* pCircleLineMesh = new CircleLine(25.f);
	TriangleMesh* pTriangleMesh = new TriangleMesh();

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	GameObject* object = CreateGameObject();
	object->AddComponent<MeshFilter>();
	object->AddComponent<Renderer>()->material = defaultMaterial;

	GameObject* scenemanager = AddGameObject();
	scenemanager->AddComponent<SceneManager>();

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

	GameObject* guardian = CreateGameObject(object);
	{
		guardian->GetComponent<MeshFilter>()->mesh = pQuadMesh;

		RevolvingBehavior* revolvingBehavior = guardian->AddComponent<RevolvingBehavior>();
		revolvingBehavior->target = star;
		revolvingBehavior->radius = 25.f;
		//revolvingBehavior->speedRotating = scenemanager->GetComponent<SceneManager>()->speedRotating;
		//revolvingBehavior->angle = scenemanager->GetComponent<SceneManager>()->angle; //0.0f * i;

		guardian->AddComponent<StarGuadian>();
		guardian->GetComponent<StarGuadian>()->bullet = bullet;
		guardian->GetComponent<StarGuadian>()->ready = true;
		//scenemanager->GetComponent<SceneManager>()->player[0] = guardian;
		SceneManager::playerprefab = guardian;
		
	}

	//GameObject* guardian = Instantiate(object);
	//{
	//	guardian->GetComponent<MeshFilter>()->mesh = pQuadMesh;

	//	RevolvingBehavior* revolvingBehavior = guardian->AddComponent<RevolvingBehavior>();
	//	revolvingBehavior->target = star;
	//	revolvingBehavior->radius = 25.f;
	//	revolvingBehavior->speedRotating = scenemanager->GetComponent<SceneManager>()->speedRotating;
	//	revolvingBehavior->angle = scenemanager->GetComponent<SceneManager>()->angle; //0.0f * i;

	//	guardian->AddComponent<StarGuadian>();
	//	guardian->GetComponent<StarGuadian>()->bullet = bullet;
	//	guardian->GetComponent<StarGuadian>()->ready = true;
	//	scenemanager->GetComponent<SceneManager>()->player[0] = guardian;
	//}
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
		//Recvthread->AddComponent<Thread>()->player = guardian;
		//Recvthread->GetComponent<Thread>()->severip = "192.168.35.95";
	}
}