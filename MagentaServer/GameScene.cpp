#include "pch.h"
#include "scripts.h"
#include "GameScene.h"

GameObject* SceneManager::scenemanager;
Scene* Scene::scene = nullptr;

void GameScene::BuildObjects()
{
	scene = this;

	GameObject* scenemanager = CreateEmpty();
	SceneManager* scnmgr = scenemanager->AddComponent<SceneManager>();

	GameObject* objectIDmanager = CreateEmpty();
	scnmgr->objectIDmanager = objectIDmanager->AddComponent<ObjectIDManager>();

	GameObject* bulletGenerator = CreateEmpty();
	scnmgr->bulletGenerator = bulletGenerator->AddComponent<BulletGenerator>();
	spawner->GetComponent<Spawner>()->objIDmgr = scnmgr->objectIDmanager;
	GameObject* object = CreateEmptyPrefab();
	GameObject* star = Instantiate(object);
	{
		star->AddComponent<Star>();
		star->AddComponent<Damageable>()->SetHealth(5);
		star->GetComponent<Damageable>()->isTeam = true;
		star->AddComponent<BoxCollider>();
		star->GetComponent<BoxCollider>()->extents = Vector3{ 5.f,5.f,1.f };
		scnmgr->star = star;
	}

	// Bullet
	{
		GameObject* bulletobj = DuplicatePrefab(object);
		bulletobj->AddComponent<Bullet>()->SetEntity(0.25f, 200.f);
		bulletGenerator->GetComponent<BulletGenerator>()->bullet[0] = bulletobj;
	}
	{
		GameObject* bulletobj = DuplicatePrefab(object);
		bulletobj->AddComponent<Bullet>()->SetEntity(0.5f, 100.f);
		bulletGenerator->GetComponent<BulletGenerator>()->bullet[1] = bulletobj;
	}
	{
		GameObject* bulletobj = DuplicatePrefab(object);
		bulletobj->AddComponent<Bullet>()->SetEntity(0.125f, 260.0f);
		bulletGenerator->GetComponent<BulletGenerator>()->bullet[2] = bulletobj;
	}
	{
		GameObject* bulletobj = DuplicatePrefab(object);
		bulletobj->AddComponent<Bullet>()->SetEntity(0.0f, 0.0f);
		bulletGenerator->GetComponent<BulletGenerator>()->bullet[3] = bulletobj;
	}
	{
		GameObject* bulletobj = DuplicatePrefab(object);
		bulletobj->AddComponent<Bullet>()->SetEntity(0.25f, 140.f);
		bulletGenerator->GetComponent<BulletGenerator>()->bullet[4] = bulletobj;
	}

	GameObject* guardian = DuplicatePrefab(object);
	{

	}

	// enemy
	GameObject* enemy0 = DuplicatePrefab(object);
	{
		enemy0->AddComponent<Enemy>();
		enemy0->AddComponent<ComingBehavior>()->target = star->GetComponent<Transform>()->position;
		enemy0->AddComponent<Damager>()->SetDamageAmount(1);
		enemy0->GetComponent<Damager>()->isTeam = false;
		enemy0->AddComponent<BoxCollider>()->extents = Vector3{ 5.f,5.f,1.f };
	}

	GameObject* enemy1 = DuplicatePrefab(object);
	{
		enemy1->AddComponent<Enemy>();
		enemy1->AddComponent<RotatingBehavior>();
		enemy1->AddComponent<MovingBehavior>()->target = star->GetComponent<Transform>()->position;
		enemy1->AddComponent<Damager>()->SetDamageAmount(1);
		enemy1->GetComponent<Damager>()->isTeam = false;
		enemy1->AddComponent<BoxCollider>()->extents = Vector3{ 5.f,5.f,5.f };
	}

	GameObject* enemy2 = DuplicatePrefab(object);
	{
		enemy2->AddComponent<Enemy>();
		enemy2->AddComponent<MovingBehavior>()->target = star->GetComponent<Transform>()->position;
		enemy2->GetComponent<MovingBehavior>()->speed = 5.f;
		enemy2->AddComponent<Damager>()->SetDamageAmount(1);
		enemy2->GetComponent<Damager>()->isTeam = false;
		enemy2->AddComponent<BoxCollider>()->extents = Vector3{ 17.5f,17.5f,17.5f };
	}

	GameObject* enemy4 = DuplicatePrefab(object);
	{
		enemy4->AddComponent<Enemy>();
		enemy4->AddComponent<WhirlingBehavior>()->target = star;
		enemy4->AddComponent<Damager>()->SetDamageAmount(1);
		enemy4->GetComponent<Damager>()->isTeam = false;
		enemy4->AddComponent<BoxCollider>()->extents = Vector3{ 5.f,5.f,5.f };
	}

	scnmgr->enemyprefab[0] = enemy0;
	scnmgr->enemyprefab[1] = enemy1;
	scnmgr->enemyprefab[2] = enemy2;
	//scnmgr->enemyprefab[3] = enemy3;
	scnmgr->enemyprefab[4] = enemy4;

	//GameObject* enemy = DuplicatePrefab(object);
	//{
	//	enemy->AddComponent<MovingBehavior>()->target = star->GetComponent<Transform>()->position;
	//	enemy->AddComponent<Damager>();
	//	enemy->GetComponent<Damager>()->SetDamageAmount(1);
	//	enemy->GetComponent<Damager>()->isTeam = false;
	//	enemy->AddComponent<BoxCollider>();
	//	enemy->GetComponent<BoxCollider>()->extents = Vector3{ 5.f,5.f,1.f };
	//}

	GameObject* spawner = CreateEmpty();
	{
		spawner->AddComponent<Spawner>()->enemy[0] = enemy0;
		spawner->GetComponent<Spawner>()->enemy[1] = enemy1;
		spawner->GetComponent<Spawner>()->enemy[2] = enemy2;
	//	spawner->GetComponent<Spawner>()->enemy[3] = enemy3;
		spawner->GetComponent<Spawner>()->enemy[4] = enemy4;
		spawner->GetComponent<Spawner>()->objIDmgr = scnmgr->objectIDmanager;
	}

}