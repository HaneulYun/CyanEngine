#pragma once
#include <list>
#include "framework.h"

struct GameObjectID
{
	int id;
	GameObject* gameobject;
};

class ObjectIDManager : public MonoBehavior<ObjectIDManager>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	std::vector<GameObjectID> ObjectsID;
public  /*이 영역에 public 변수를 선언하세요.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<ObjectIDManager>;
	ObjectIDManager() = default;
	ObjectIDManager(ObjectIDManager&) = default;

public:
	~ObjectIDManager() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
	}

	void CreateObjectID(int id, GameObject* obj)
	{
		obj->GetComponent<ObjectID>()->SetObjectID(id);
		ObjectsID.push_back(GameObjectID{ id, obj });
	}

	void DeleteObjectID(int id)
	{
		for (auto iter = ObjectsID.begin(); iter != ObjectsID.end();)
		{
			if (iter->id == id)
			{
				Destroy(iter->gameobject);
				iter = ObjectsID.erase(iter);
			}
			else
				++iter;
		}
	}

	void CollideBulletandEnemy(int bulletID, int enemyID, int damage)
	{
		for (auto iter = ObjectsID.begin(); iter != ObjectsID.end();)
		{
			if (iter->id == bulletID)
			{
				Destroy(iter->gameobject);
				iter = ObjectsID.erase(iter);
			}
			else if (iter->id == enemyID)
			{
				Damageable* damageable = iter->gameobject->GetComponent<Damageable>();
				damageable->TakeDamage(damage);
				if (damageable->isDead())
				{
					Destroy(iter->gameobject);
					iter = ObjectsID.erase(iter);
				}
				else ++iter;
			}
			else 
				++iter;
		}
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

