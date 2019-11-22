#include "pch.h"
#include "GameScene.h"

GameObject* SceneManager::scenemanager;


Scene* Scene::scene{ nullptr };

void GameScene::BuildObjects()
{
	scene = this;
	Quad* pQuadMesh = new Quad(10.0f, 10.0f);
	Circle* pCircleMesh = new Circle(10, 48);
	CircleLine* pCircleLineMesh = new CircleLine(25.f);
	TriangleMesh* pTriangleMesh = new TriangleMesh();

	Material* defaultMaterial = new DefaultMaterial();
	defaultMaterial->shader = new StandardShader();

	GameObject* object = CreateGameObject();
	object->AddComponent<MeshFilter>();
	object->AddComponent<Renderer>()->material = defaultMaterial;

	GameObject* scenemanager = CreateEmpty();
	SceneManager* scnmgr = scenemanager->AddComponent<SceneManager>();

	GameObject* star = Instantiate(object);
	{
		star->GetComponent<MeshFilter>()->mesh = pCircleMesh;
		star->AddComponent<Star>();
		scnmgr->star = star;
	}

	GameObject* orbit = Instantiate(object);
	{
		orbit->GetComponent<MeshFilter>()->mesh = pCircleLineMesh;
	}

	GameObject* bullet = CreateGameObject(object);
	bullet->GetComponent<MeshFilter>()->mesh = pTriangleMesh;
	bullet->AddComponent<Bullet>();


	GameObject* guardian = CreateGameObject(object);
	{
		guardian->GetComponent<MeshFilter>()->mesh = pQuadMesh;

		/*RevolvingBehavior* revolvingBehavior = guardian->AddComponent<RevolvingBehavior>();
		revolvingBehavior->target = star;
		revolvingBehavior->radius = 25.f;*/

		guardian->AddComponent<StarGuadian>();
		guardian->GetComponent<StarGuadian>()->bullet = bullet;
		scnmgr->playerprefab = guardian;
		
	}

	GameObject* enemy = CreateGameObject(object);
	{
		enemy->GetComponent<MeshFilter>()->mesh = pQuadMesh;
		enemy->AddComponent<MovingBehavior>()->target = star->GetComponent<Transform>()->position;
	}
	
	GameObject* spawner = CreateEmpty();
	{
		spawner->AddComponent<Spawner>()->enemy = enemy;
	}

	GameObject* Recvthread = CreateEmpty(); {
		//Recvthread->AddComponent<Thread>()->severip = "192.168.22.159";
		//Recvthread->GetComponent<Thread>()
	}
}