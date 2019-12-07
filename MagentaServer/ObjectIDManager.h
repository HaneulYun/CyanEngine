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
		for (auto iter = ObjectsID.begin(); iter != ObjectsID.end(); ++iter)
		{
			if (iter->id == id)
			{
				iter->gameobject->Destroy(iter->gameobject);
				ObjectsID.erase(iter);
				break;
			}
		}
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

