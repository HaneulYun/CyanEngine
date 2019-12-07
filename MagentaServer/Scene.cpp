#include "pch.h"
#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Start()
{
	BuildObjects();

	for (int i = 0; i < gameObjects.size(); ++i)
		gameObjects[i]->Start();
}

void Scene::Update()
{
	// fixed update
	BoxCollider* lhs_collider, * rhs_collider;
	for (auto lhs_iter = gameObjects.begin(); lhs_iter != gameObjects.end(); ++lhs_iter)
		if (lhs_collider = (*lhs_iter)->GetComponent<BoxCollider>())
			for (auto rhs_iter = lhs_iter + 1; rhs_iter != gameObjects.end(); ++rhs_iter)
				if (rhs_collider = (*rhs_iter)->GetComponent<BoxCollider>())
				{
					auto iter = (*lhs_iter)->collisionType.find(*rhs_iter);

					if (lhs_collider->Compare(rhs_collider))
					{
						if (iter == (*lhs_iter)->collisionType.end())
							((*lhs_iter)->collisionType)[*rhs_iter] = CollisionType::eTriggerEnter;
						else if (iter->second == CollisionType::eTriggerEnter)
							iter->second = CollisionType::eTriggerStay;
					}
					else if (iter != (*lhs_iter)->collisionType.end())
						iter->second = CollisionType::eTriggerExit;
				}

	for (GameObject* gameObject : gameObjects)
	{
		for (auto d : gameObject->collisionType)
		{
			switch (d.second)
			{
			case CollisionType::eCollisionEnter:
				gameObject->OnCollisionEnter(d.first); break;
			case CollisionType::eCollisionStay:
				gameObject->OnCollisionStay(d.first); break;
			case CollisionType::eCollisionExit:
				gameObject->OnCollisionExit(d.first); break;
			case CollisionType::eTriggerEnter:
				gameObject->OnTriggerEnter(d.first); break;
			case CollisionType::eTriggerStay:
				gameObject->OnTriggerStay(d.first); break;
			case CollisionType::eTriggerExit:
				gameObject->OnTriggerExit(d.first); break;
			}
		}
	}

	for (GameObject* gameObject : gameObjects)
		gameObject->Update();

	while (!deletionQueue.empty())
	{
		GameObject* gameObject = deletionQueue.top();
		Delete(gameObject);
		deletionQueue.pop();
	}
}

void Scene::BuildObjects()
{
}

void Scene::ReleaseObjects()
{
	for (GameObject* object : gameObjects)
		delete object;
	gameObjects.clear();
}

GameObject* Scene::CreateEmpty()
{
	GameObject* gameObject = new GameObject();
	gameObject->scene = this;
	gameObjects.push_back(gameObject);

	return gameObject;
}

GameObject* Scene::Duplicate(GameObject* _gameObject)
{
	GameObject* gameObject = new GameObject(_gameObject);
	gameObject->scene = this;
	gameObjects.push_back(gameObject);

	return gameObject;
}

GameObject* Scene::CreateEmptyPrefab()
{
	GameObject* gameObject = new GameObject();
	gameObject->scene = this;

	return gameObject;
}

GameObject* Scene::DuplicatePrefab(GameObject* _gameObject)
{
	GameObject* gameObject = new GameObject(_gameObject);
	gameObject->scene = this;

	return gameObject;
}

void Scene::PushDelete(GameObject* gameObject)
{
	deletionQueue.push(gameObject);
}

void Scene::Delete(GameObject* gameObject)
{
	for (auto iter = gameObjects.begin(); iter != gameObjects.end(); ++iter)
		if (*iter == gameObject)
		{
			delete (*iter);
			gameObjects.erase(iter);
			return;
		}
}