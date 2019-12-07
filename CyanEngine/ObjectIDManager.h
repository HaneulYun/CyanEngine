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
		for (auto iter = ObjectsID.begin(); iter != ObjectsID.end(); ++iter)
		{
			//if (iter->gameobject == nullptr)
			//{
			//	ObjectsID.erase(iter);
			//	break;
			//}
			if (iter->id == id)
			{
				Destroy(iter->gameobject);
				ObjectsID.erase(iter);
				break;
			}
		}
	}

	void CollideBulletandEnemy(int bulletID, int enemyID, int damage)
	{
		for (auto iter = ObjectsID.begin(); iter != ObjectsID.end(); ++iter)
		{
			if (iter->id == bulletID)
			{
				Destroy(iter->gameobject);
				ObjectsID.erase(iter);
			}
			else if (iter->id == enemyID)
			{
				Damageable* damagealbe = iter->gameobject->GetComponent<Damageable>();
				damagealbe->TakeDamage(damage);
				if (damagealbe->isDead())
				{
					Destroy(iter->gameobject);
					ObjectsID.erase(iter);
				}
			}
		}
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

