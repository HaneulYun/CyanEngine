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
private /*�� ������ private ������ �����ϼ���.*/:
	std::vector<GameObjectID> ObjectsID;
public  /*�� ������ public ������ �����ϼ���.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<ObjectIDManager>;
	ObjectIDManager() = default;
	ObjectIDManager(ObjectIDManager&) = default;

public:
	~ObjectIDManager() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
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

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

