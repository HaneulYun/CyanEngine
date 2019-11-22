#include "pch.h"
#include "GameScene.h"

Scene* Scene::scene{ nullptr };

void GameScene::BuildObjects()
{
	scene = this;

	GameObject* object = CreateGameObject();
	GameObject* scenemanager = CreateEmpty();
	
	GameObject* star = Instantiate(object);
	{
		star->AddComponent<Star>();
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
		//enemy->AddComponent<MovingBehavior>()->target = star->GetComponent<Transform>()->position;
	}

	GameObject* spawner = CreateEmpty();
	{
		//spawner->AddComponent<Spawner>()->enemy = enemy;
	}

}