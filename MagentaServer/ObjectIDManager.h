#pragma once
#include <list>

struct GameObjectID
{
	int id;
	GameObject* gameobject;
};

class ObjectIDManager : public MonoBehavior<ObjectIDManager>
{
private /*�� ������ private ������ �����ϼ���.*/:
	int curID;
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
		curID = 0;
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	//void CreateObjectID(int id, GameObject* obj)
	//{
	//	obj->GetComponent<ObjectID>()->SetObjectID(id);
	//	ObjectsID.push_back(GameObjectID{ id, obj });
	//	curID++;
	//}

	void CreateObjectID(GameObject* obj)
	{
		obj->AddComponent<ObjectID>()->SetObjectID(curID);
		ObjectsID.push_back(GameObjectID{ curID, obj });
		curID++;
	}

	void DeleteObjectID(int id)
	{
		for (auto iter = ObjectsID.begin(); iter != ObjectsID.end();)
		{
			if (iter->id == id)
			{
				Destroy(iter->gameobject);
				iter = ObjectsID.erase(iter);
				break;
			}
			else
				++iter;
		}
	}

	GameObject* GetGameObject(int id)
	{
		for (auto iter = ObjectsID.begin(); iter != ObjectsID.end();)
		{
			if (iter->id == id)
				return iter->gameobject;
			else
				++iter;
		}
		return nullptr;
	}
	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

