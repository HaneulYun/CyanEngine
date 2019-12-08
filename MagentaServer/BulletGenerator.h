#pragma once

class BulletGenerator : public MonoBehavior<BulletGenerator>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	GameObject* bullet[5]{ nullptr };
	ObjectIDManager* objIDmgr{ nullptr };

private:
	friend class GameObject;
	friend class MonoBehavior<BulletGenerator>;
	BulletGenerator() = default;
	BulletGenerator(BulletGenerator&) = default;

public:
	~BulletGenerator() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	int CreateBulletAndGetObjID(int type)
	{
		if (SceneManager::scenemanager->gameState == START) 
		{
			GameObject* object = Instantiate(bullet[type]);
			objIDmgr->CreateObjectID(object);
			return object->GetComponent<ObjectID>()->GetObjectID();
		}
		return -1;
	}
	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

