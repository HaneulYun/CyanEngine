#include "pch.h"
#include "scripts.h"
#include "GameScene.h"

GameObject* SceneManager::scenemanager;
Scene* Scene::scene = nullptr;

void GameScene::BuildObjects()
{
	scene = this;

	GameObject* object = CreateGameObject();
	GameObject* scenemanager = CreateEmpty();
	SceneManager* scnmgr = scenemanager->AddComponent<SceneManager>();
	
	GameObject* star = Instantiate(object);
	{
		star->AddComponent<Star>();
		star->AddComponent<Damageable>()->SetHealth(5);
		star->GetComponent<Damageable>()->isTeam = true;
		star->AddComponent<BoxCollider>();
		star->GetComponent<BoxCollider>()->extents = Vector3{ 5.f,5.f,5.f };
		scnmgr->star = star;
	}

	GameObject* bullet = CreateGameObject(object);
	{
		bullet->AddComponent<Bullet>();
	}

	GameObject* guardian = CreateGameObject(object);
	{

	}

	GameObject* enemy = CreateGameObject(object);
	{
		enemy->AddComponent<MovingBehavior>()->target = star->GetComponent<Transform>()->position;
		enemy->AddComponent<Damager>();
		enemy->GetComponent<Damager>()->SetDamageAmount(1);
		enemy->GetComponent<Damager>()->isTeam = false;
		enemy->AddComponent<BoxCollider>();
		enemy->GetComponent<BoxCollider>()->extents = Vector3{ 5.f,5.f,5.f };
	}

	GameObject* spawner = CreateEmpty();
	{
		spawner->AddComponent<Spawner>()->enemy = enemy;
	}

}