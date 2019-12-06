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

	GameObject* mainCamera = CreateEmpty();
	{
		Camera* camera = mainCamera->AddComponent<Camera>();
		camera->SetViewport(0, 0, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight, 0.0f, 1.0f);
		camera->SetScissorRect(0, 0, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight);
		camera->GenerateProjectionMatrix(0.3f, 1000.0f, float(CyanWindow::m_nWndClientWidth) / float(CyanWindow::m_nWndClientHeight), 90.0f);
		camera->GenerateViewMatrix(XMFLOAT3(0.0f, 0.0f, -10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		camera->main = camera;
	}

	GameObject* object = CreateEmptyPrefab();
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
		GameObject* bulletobj = DuplicatePrefab(object);
		bulletobj->GetComponent<MeshFilter>()->mesh = BulletMesh;
		bulletobj->AddComponent<Bullet>()->SetEntity(0.25f, 200.f);
		scnmgr->bulletprefab[0] = bulletobj;
		//bulletobj->AddComponent<Damager>()->isTeam = true;
		//bulletobj->GetComponent<Damager>()->SetDamageAmount(1);
		//bulletobj->AddComponent<BoxCollider>()->extents = Vector3{ 1.5f,1.5f,1.5f };
	}
	{
		GameObject* bulletobj = DuplicatePrefab(object);
		bulletobj->GetComponent<MeshFilter>()->mesh = CannonMesh;
		bulletobj->AddComponent<Bullet>()->SetEntity(0.5f, 100.f);
		scnmgr->bulletprefab[1] = bulletobj;
		//bulletobj->AddComponent<Damager>()->isTeam = true;
		//bulletobj->GetComponent<Damager>()->SetDamageAmount(5);
		//bulletobj->AddComponent<SphereCollider>()->radius = 4.f;
	} 
	{	
		GameObject* bulletobj = DuplicatePrefab(object);
		bulletobj->GetComponent<MeshFilter>()->mesh = SharpBulletMesh;
		bulletobj->AddComponent<Bullet>()->SetEntity(0.125f, 260.0f);
		scnmgr->bulletprefab[2] = bulletobj;
		//bulletobj->AddComponent<Damager>()->isTeam = true;
		//bulletobj->GetComponent<Damager>()->SetDamageAmount(0.5);
		//bulletobj->AddComponent<BoxCollider>()->extents = Vector3{ 1.5f,1.5f,1.5f };
	} 
	{	
		GameObject* bulletobj = DuplicatePrefab(object);
		bulletobj->GetComponent<MeshFilter>()->mesh = BulletMesh;
		bulletobj->AddComponent<Bullet>()->SetEntity(0.0f, 0.0f);
		scnmgr->bulletprefab[3] = bulletobj;
		//bulletobj->AddComponent<Damager>()->isTeam = true;
		//bulletobj->GetComponent<Damager>()->SetDamageAmount(1);
		//bulletobj->AddComponent<BoxCollider>()->extents = Vector3{ 1.5f,1.5f,1.5f };
	} 
	{	
		GameObject* bulletobj = DuplicatePrefab(object);
		bulletobj->GetComponent<MeshFilter>()->mesh = BulletMesh;
		bulletobj->AddComponent<Bullet>()->SetEntity(0.25f, 140.f);
		scnmgr->bulletprefab[4] = bulletobj;
		//bulletobj->AddComponent<Damager>()->isTeam = true;
		//bulletobj->GetComponent<Damager>()->SetDamageAmount(1);
		//bulletobj->AddComponent<BoxCollider>()->extents = Vector3{ 1.5f,1.5f,1.5f };
	}
	
	GameObject* guardian = DuplicatePrefab(object);
	{
		guardian->GetComponent<MeshFilter>()->mesh = pQuadMesh;

		/*RevolvingBehavior* revolvingBehavior = guardian->AddComponent<RevolvingBehavior>();
		revolvingBehavior->target = star;
		revolvingBehavior->radius = 25.f;*/

		guardian->AddComponent<StarGuardian>();
		scnmgr->playerprefab = guardian;
	}


	// enemy
	GameObject* enemy0 = DuplicatePrefab(object);
	{
		enemy0->GetComponent<MeshFilter>()->mesh = pQuadMesh;
		enemy0->AddComponent<Enemy>();
		enemy0->AddComponent<MovingBehavior>()->target = star->GetComponent<Transform>()->position;
		enemy0->AddComponent<Damageable>()->SetHealth(2);
		enemy0->GetComponent<Damageable>()->isTeam = false;
		enemy0->AddComponent<BoxCollider>()->extents = Vector3{ 5.f,5.f,1.f };
	}

	GameObject* enemy1 = DuplicatePrefab(object);
	{
		enemy1->GetComponent<MeshFilter>()->mesh = pQuadMesh;
		enemy1->AddComponent<Enemy>();
		enemy1->AddComponent<WhirlingBehavior>()->target = star;
		//enemy1->AddComponent<Damageable>()->SetHealth(4);
		//enemy1->GetComponent<Damageable>()->isTeam = false;
		//enemy1->AddComponent<BoxCollider>()->extents = Vector3{ 10.f,10.f,10.f };
	}

	scnmgr->enemyprefab[0] = enemy0;
	//scnmgr->enemyprefab[1] = enemy1;
	

	GameObject* spawner = CreateEmpty();
	{
		//spawner->AddComponent<Spawner>()->enemy = enemy0;
	}


	GameObject* Recvthread = CreateEmpty();
	{
		//Recvthread->AddComponent<Thread>()->severip = "192.168.22.163";
		//Recvthread->AddComponent<Thread>()->severip = "192.168.35.35";
		//Recvthread->AddComponent<Thread>()->severip = "192.168.21.141";
		Recvthread->AddComponent<Thread>()->severip = "127.0.0.1";
		scnmgr->Sender = Recvthread->GetComponent<Thread>();
	}

	GameObject* orbit = Instantiate(object);
	{
		orbit->GetComponent<MeshFilter>()->mesh = pCircleLineMesh;
	}
}