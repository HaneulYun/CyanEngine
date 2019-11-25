#include "pch.h"
#include "GameScene.h"

GameObject* SceneManager::scenemanager{ nullptr };
Scene* Scene::scene{ nullptr };

void GameScene::BuildObjects()
{
	scene = this;
	Quad* pQuadMesh = new Quad(10.0f, 10.0f);
	Circle* pCircleMesh = new Circle(10, 48);
	CircleLine* pCircleLineMesh = new CircleLine(25.f);
	TriangleMesh* BulletMesh = new TriangleMesh(3.0f);
	TriangleMesh* SharpBulletMesh = new TriangleMesh(1.0f);
	Circle* CannonMesh = new Circle(4, 48);

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

	
	{	// StraightBullet
		GameObject* bulletobj = CreateGameObject(object);
		bulletobj->GetComponent<MeshFilter>()->mesh = BulletMesh;
		Bullet* bullet = bulletobj->AddComponent<Bullet>();
		bullet->speed = 200.f;
		bullet->timeCycle = 0.5f;
		scnmgr->bulletprefab[0] = bulletobj;
	}
	{	
		GameObject* bulletobj = CreateGameObject(object);
		bulletobj->GetComponent<MeshFilter>()->mesh = CannonMesh;
		Bullet* bullet = bulletobj->AddComponent<Bullet>();
		bullet->speed = 100.f;
		bullet->timeCycle = 1.0f;
		scnmgr->bulletprefab[1] = bulletobj;
	} 
	{	
		GameObject* bulletobj = CreateGameObject(object);
		bulletobj->GetComponent<MeshFilter>()->mesh = SharpBulletMesh;
		Bullet* bullet = bulletobj->AddComponent<Bullet>();
		bullet->speed = 260.f;
		bullet->timeCycle = 0.25f;
		scnmgr->bulletprefab[2] = bulletobj;
	} 
	{	
		GameObject* bulletobj = CreateGameObject(object);
		bulletobj->GetComponent<MeshFilter>()->mesh = BulletMesh;
		Bullet* bullet = bulletobj->AddComponent<Bullet>();
		bullet->speed = 0.f;
		bullet->timeCycle = 0.0f;
		scnmgr->bulletprefab[3] = bulletobj;
	} 
	{	
		GameObject* bulletobj = CreateGameObject(object);
		bulletobj->GetComponent<MeshFilter>()->mesh = BulletMesh;
		Bullet* bullet = bulletobj->AddComponent<Bullet>();
		bullet->speed = 140.f;
		bullet->timeCycle = 0.5f;
		scnmgr->bulletprefab[4] = bulletobj;
	}

	GameObject* guardian = CreateGameObject(object);
	{
		guardian->GetComponent<MeshFilter>()->mesh = pQuadMesh;

		/*RevolvingBehavior* revolvingBehavior = guardian->AddComponent<RevolvingBehavior>();
		revolvingBehavior->target = star;
		revolvingBehavior->radius = 25.f;*/

		guardian->AddComponent<StarGuardian>();
		scnmgr->playerprefab = guardian;
	 }

	// 서버 연결X
	/*for (int i = 0; i < 3; ++i) {
		GameObject* guardian = Instantiate(object);
		guardian->GetComponent<MeshFilter>()->mesh = pQuadMesh;

		RevolvingBehavior* revolvingBehavior = guardian->AddComponent<RevolvingBehavior>();
		revolvingBehavior->target = star;
		revolvingBehavior->radius = 25.f;
		revolvingBehavior->angle = 120 * i;

		guardian->AddComponent<StarGuardian>();
		guardian->GetComponent<StarGuardian>()->bullet = bullet;
		scnmgr->player[i] = guardian;
	}*/
	

	GameObject* enemy0 = CreateGameObject(object);
	{
		enemy0->GetComponent<MeshFilter>()->mesh = pQuadMesh;
		enemy0->AddComponent<MovingBehavior>()->target = star->GetComponent<Transform>()->position;
	 }

	GameObject* enemy1 = CreateGameObject(object);
	{
		enemy1->GetComponent<MeshFilter>()->mesh = pQuadMesh;
		enemy1->AddComponent<WhirlingBehavior>()->target = star;
	}

	scnmgr->enemyprefab[0] = enemy0;
	
	GameObject* spawner = CreateEmpty();
	{
		//spawner->AddComponent<Spawner>()->enemy = enemy1;
	}

	GameObject* Recvthread = CreateEmpty();
	{
		//Recvthread->AddComponent<Thread>()->severip = "192.168.35.35";
		Recvthread->AddComponent<Thread>()->severip = "192.168.100.77";
		scnmgr->Sender = Recvthread->GetComponent<Thread>();
	}

	GameObject* orbit = Instantiate(object);
	{
		orbit->GetComponent<MeshFilter>()->mesh = pCircleLineMesh;
	}
}