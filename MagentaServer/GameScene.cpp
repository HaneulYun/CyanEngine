#include "pch.h"
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
	}

	GameObject* spawner = CreateEmpty();
	{
		spawner->AddComponent<Spawner>()->enemy = enemy;
	}

}