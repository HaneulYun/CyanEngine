#include "pch.h"
#include "scripts.h"
#include "GameScene.h"

GameObject* SceneManager::scenemanager{ nullptr };
Scene* Scene::scene{ nullptr };

std::queue<Message> recvQueue;
std::queue<Message> sendQueue;
CRITICAL_SECTION rqcs;

Mesh* Bullet::mesh{ nullptr };

void GameScene::BuildObjects()
{
	scene = this;

	// preComponent
	Bullet::mesh = new Quad(2.0f, 3.0f);

	Quad* pQuadMesh = new Quad(10.0f, 10.0f);
	Quad* BigMesh = new Quad(25.0f, 25.0f);
	Quad* LaserMesh = new Quad(2.0f, 360.0f);
	Circle* pCircleMesh = new Circle(10, 48);
	CircleLine* pCircleLineMesh = new CircleLine(25.f);
	TriangleMesh* BulletMesh = new TriangleMesh(3.0f);
	TriangleMesh* SharpBulletMesh = new TriangleMesh(1.0f);
	Circle* CannonMesh = new Circle(4, 48);

	Material* defaultMaterial = new Material();

	// main camera
	GameObject* mainCamera = CreateEmpty();
	{
		Camera* camera = mainCamera->AddComponent<Camera>();
		camera->SetViewport(0, 0, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight, 0.0f, 1.0f);
		camera->SetScissorRect(0, 0, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight);
		camera->GenerateProjectionMatrix(0.3f, 1000.0f, float(CyanWindow::m_nWndClientWidth) / float(CyanWindow::m_nWndClientHeight), 90.0f);
		camera->GenerateViewMatrix(XMFLOAT3(0.0f, 0.0f, -10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		camera->main = camera;
	}

	// defulat prefab
	GameObject* object = CreateEmptyPrefab();
	object->AddComponent<MeshFilter>();
	object->AddComponent<Renderer>()->material = defaultMaterial;

	GameObject* scenemanager = CreateEmpty();
	SceneManager* scnmgr = scenemanager->AddComponent<SceneManager>();

	GameObject* objectIDmanager = CreateEmpty();
	scnmgr->objectIDmanager = objectIDmanager->AddComponent<ObjectIDManager>();
	
	GameObject* star = Instantiate(object);
	{
		star->GetComponent<MeshFilter>()->mesh = pCircleMesh;
		star->AddComponent<Star>();
		scnmgr->star = star;
	}
	
	// 5 Bullet
	GameObject* bulletPrefab = DuplicatePrefab(object);
	{
		bulletPrefab->AddComponent<Bullet>();
		bulletPrefab->AddComponent<AfterImageGenerator>();
		bulletPrefab->AddComponent<ObjectID>();
	}
	{	
		GameObject* bulletobj = DuplicatePrefab(bulletPrefab);
		bulletobj->GetComponent<MeshFilter>()->mesh = BulletMesh;
		bulletobj->GetComponent<Bullet>()->SetEntity(0.2f, 200.f);
		bulletobj->GetComponent<AfterImageGenerator>()->SetEntity(0.1f, 0.5f);
		scnmgr->bulletprefab[0] = bulletobj;
	}
	{
		GameObject* bulletobj = DuplicatePrefab(bulletPrefab);
		bulletobj->GetComponent<MeshFilter>()->mesh = CannonMesh;
		bulletobj->GetComponent<Bullet>()->SetEntity(0.3f, 100.f);
		scnmgr->bulletprefab[1] = bulletobj;
	} 
	{	
		GameObject* bulletobj = DuplicatePrefab(bulletPrefab);
		bulletobj->GetComponent<MeshFilter>()->mesh = SharpBulletMesh;
		bulletobj->GetComponent<Bullet>()->SetEntity(0.1f, 260.0f);
		scnmgr->bulletprefab[2] = bulletobj;
	} 
	{	
		GameObject* bulletobj = DuplicatePrefab(bulletPrefab);
		bulletobj->GetComponent<MeshFilter>()->mesh = LaserMesh;
		bulletobj->GetComponent<Bullet>()->SetEntity(0.3f, 0.0f, 0.3f);
		scnmgr->bulletprefab[3] = bulletobj;
	} 
	{	
		GameObject* bulletobj = DuplicatePrefab(bulletPrefab);
		bulletobj->GetComponent<MeshFilter>()->mesh = BulletMesh;
		bulletobj->GetComponent<Bullet>()->SetEntity(0.25f, 140.f);
		scnmgr->bulletprefab[4] = bulletobj;
	}
	
	GameObject* guardian = DuplicatePrefab(object);
	{
		guardian->GetComponent<MeshFilter>()->mesh = pQuadMesh;

		/*RevolvingBehavior* revolvingBehavior = guardian->AddComponent<RevolvingBehavior>();
		revolvingBehavior->target = star;
		revolvingBehavior->radius = 25.f;*/

		guardian->AddComponent<StarGuardian>();
		guardian->AddComponent<AfterImageGenerator>();
		scnmgr->playerprefab = guardian;
	}


	// enemy
	GameObject* enemy0 = DuplicatePrefab(object);
	{
		enemy0->GetComponent<MeshFilter>()->mesh = pQuadMesh;
		enemy0->AddComponent<Enemy>();
		enemy0->AddComponent<ComingBehavior>()->target = star->GetComponent<Transform>()->position;
		enemy0->AddComponent<Damageable>()->SetHealth(4);
		enemy0->GetComponent<Damageable>()->isTeam = false;
		enemy0->AddComponent<BoxCollider>()->extents = Vector3{ 5.f,5.f,1.f };
		enemy0->GetComponent<BoxCollider>()->obb = true;
		enemy0->AddComponent<AfterImageGenerator>();
		enemy0->AddComponent<ObjectID>();
	}

	GameObject* enemy1 = DuplicatePrefab(object);
	{
		enemy1->GetComponent<MeshFilter>()->mesh = pQuadMesh;
		enemy1->AddComponent<Enemy>();
		enemy1->AddComponent<RotatingBehavior>();
		enemy1->AddComponent<MovingBehavior>()->target = star->GetComponent<Transform>()->position;
		enemy1->AddComponent<Damageable>()->SetHealth(8);
		enemy1->GetComponent<Damageable>()->isTeam = false;
		enemy1->AddComponent<BoxCollider>()->extents = Vector3{ 5.f,5.f,5.f };
		enemy1->AddComponent<AfterImageGenerator>();
		enemy1->AddComponent<ObjectID>();
	}

	GameObject* enemy2 = DuplicatePrefab(object);
	{
		enemy2->GetComponent<MeshFilter>()->mesh = BigMesh;
		enemy2->AddComponent<Enemy>();
		enemy2->AddComponent<MovingBehavior>()->target = star->GetComponent<Transform>()->position;
		enemy2->GetComponent<MovingBehavior>()->speed = 8.f;
		enemy2->AddComponent<Damageable>()->SetHealth(32);
		enemy2->GetComponent<Damageable>()->isTeam = false;
		enemy2->AddComponent<BoxCollider>()->extents = Vector3{ 17.5f,17.5f,17.5f };
		enemy2->AddComponent<AfterImageGenerator>();
		enemy2->AddComponent<ObjectID>();
	}

	GameObject* enemy4 = DuplicatePrefab(object);
	{
		enemy4->GetComponent<MeshFilter>()->mesh = pQuadMesh;
		enemy4->AddComponent<Enemy>();
		enemy4->AddComponent<WhirlingBehavior>()->target = star;
		enemy4->AddComponent<Damageable>()->SetHealth(16);
		enemy4->GetComponent<Damageable>()->isTeam = false;
		enemy4->AddComponent<BoxCollider>()->extents = Vector3{ 5.f,5.f,5.f };
		enemy4->AddComponent<AfterImageGenerator>();
		enemy4->AddComponent<ObjectID>();
	}

	scnmgr->enemyprefab[0] = enemy0;
	scnmgr->enemyprefab[1] = enemy1;
	scnmgr->enemyprefab[2] = enemy2;
	//scnmgr->enemyprefab[3] = enemy3;
	scnmgr->enemyprefab[4] = enemy4;
	

	GameObject* spawner = CreateEmpty();
	{
		//spawner->AddComponent<Spawner>()->enemy = enemy0;
	}


	GameObject* Recvthread = CreateEmpty();
	{
		//Recvthread->AddComponent<Thread>()->severip = "192.168.22.163";
		//Recvthread->AddComponent<Thread>()->severip = "192.168.35.35";
		//Recvthread->AddComponent<Thread>()->severip = "192.168.21.141";
		//Recvthread->AddComponent<Thread>()->severip = "192.168.100.109";
		Recvthread->AddComponent<Thread>()->severip = "127.0.0.1";
		scnmgr->Sender = Recvthread->GetComponent<Thread>();
	}

	GameObject* orbit = Instantiate(object);
	{
		orbit->GetComponent<MeshFilter>()->mesh = pCircleLineMesh;
	}
}