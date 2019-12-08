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

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

