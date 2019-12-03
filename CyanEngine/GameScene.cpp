#include "pch.h"
#include "scripts.h"
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
	
	// 5 Bullet
	{	
		GameObject* bulletobj = CreateGameObject(object);
		bulletobj->GetComponent<MeshFilter>()->mesh = BulletMesh;
		Bullet* bullet = bulletobj->AddComponent<Bullet>();
		bullet->speed = 200.f;
		bullet->timeCycle = 0.25f;
		scnmgr->bulletprefab[0] = bulletobj;
		//bulletobj->AddComponent<Damager>()->isTeam = true;
		//bulletobj->GetComponent<Damager>()->SetDamageAmount(1);
		//bulletobj->AddComponent<BoxCollider>()->extents = Vector3{ 1.5f,1.5f,1.5f };
	}
	{	
		GameObject* bulletobj = CreateGameObject(object);
		bulletobj->GetComponent<MeshFilter>()->mesh = CannonMesh;
		Bullet* bullet = bulletobj->AddComponent<Bullet>();
		bullet->speed = 100.f;
		bullet->timeCycle = 0.5f;
		scnmgr->bulletprefab[1] = bulletobj;
		//bulletobj->AddComponent<Damager>()->isTeam = true;
		//bulletobj->GetComponent<Damager>()->SetDamageAmount(5);
		//bulletobj->AddComponent<SphereCollider>()->radius = 4.f;
	} 
	{	
		GameObject* bulletobj = CreateGameObject(object);
		bulletobj->GetComponent<MeshFilter>()->mesh = SharpBulletMesh;
		Bullet* bullet = bulletobj->AddComponent<Bullet>();
		bullet->speed = 260.f;
		bullet->timeCycle = 0.125f;
		scnmgr->bulletprefab[2] = bulletobj;
		//bulletobj->AddComponent<Damager>()->isTeam = true;
		//bulletobj->GetComponent<Damager>()->SetDamageAmount(0.5);
		//bulletobj->AddComponent<BoxCollider>()->extents = Vector3{ 1.5f,1.5f,1.5f };
	} 
	{	
		GameObject* bulletobj = CreateGameObject(object);
		bulletobj->GetComponent<MeshFilter>()->mesh = BulletMesh;
		Bullet* bullet = bulletobj->AddComponent<Bullet>();
		bullet->speed = 0.f;
		bullet->timeCycle = 0.0f;
		scnmgr->bulletprefab[3] = bulletobj;
		//bulletobj->AddComponent<Damager>()->isTeam = true;
		//bulletobj->GetComponent<Damager>()->SetDamageAmount(1);
		//bulletobj->AddComponent<BoxCollider>()->extents = Vector3{ 1.5f,1.5f,1.5f };
	} 
	{	
		GameObject* bulletobj = CreateGameObject(object);
		bulletobj->GetComponent<MeshFilter>()->mesh = BulletMesh;
		Bullet* bullet = bulletobj->AddComponent<Bullet>();
		bullet->speed = 140.f;
		bullet->timeCycle = 0.25f;
		scnmgr->bulletprefab[4] = bulletobj;
		//bulletobj->AddComponent<Damager>()->isTeam = true;
		//bulletobj->GetComponent<Damager>()->SetDamageAmount(1);
		//bulletobj->AddComponent<BoxCollider>()->extents = Vector3{ 1.5f,1.5f,1.5f };
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


	// enemy
	GameObject* enemy0 = CreateGameObject(object);
	{
		enemy0->GetComponent<MeshFilter>()->mesh = pQuadMesh;
		enemy0->AddComponent<MovingBehavior>()->target = star->GetComponent<Transform>()->position;
		enemy0->AddComponent<Damageable>()->SetHealth(2);
		enemy0->GetComponent<Damageable>()->isTeam = false;
		enemy0->AddComponent<BoxCollider>()->extents = Vector3{ 10.f,10.f,10.f };
	}

	GameObject* enemy1 = CreateGameObject(object);
	{
		enemy1->GetComponent<MeshFilter>()->mesh = pQuadMesh;
		enemy1->AddComponent<WhirlingBehavior>()->target = star;
		//enemy1->AddComponent<Damageable>()->SetHealth(4);
		//enemy1->GetComponent<Damageable>()->isTeam = false;
		//enemy1->AddComponent<BoxCollider>()->extents = Vector3{ 10.f,10.f,10.f };
	}

	scnmgr->enemyprefab[0] = enemy0;
	scnmgr->enemyprefab[1] = enemy1;
	

	GameObject* spawner = CreateEmpty();
	{
		//spawner->AddComponent<Spawner>()->enemy = enemy1;
	}


	GameObject* Recvthread = CreateEmpty();
	{
		//Recvthread->AddComponent<Thread>()->severip = "192.168.22.163";
		//Recvthread->AddComponent<Thread>()->severip = "192.168.35.35";
		//Recvthread->AddComponent<Thread>()->severip = "192.168.100.77";
		Recvthread->AddComponent<Thread>()->severip = "127.0.0.1";
		scnmgr->Sender = Recvthread->GetComponent<Thread>();
	}

	GameObject* orbit = Instantiate(object);
	{
		orbit->GetComponent<MeshFilter>()->mesh = pCircleLineMesh;
	}
}