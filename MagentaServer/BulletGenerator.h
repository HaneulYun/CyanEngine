#pragma once

class BulletGenerator : public MonoBehavior<BulletGenerator>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	GameObject* bullet[5]{ nullptr };
	GameObject* objIDmgr{ nullptr };

private:
	friend class GameObject;
	friend class MonoBehavior<BulletGenerator>;
	BulletGenerator() = default;
	BulletGenerator(BulletGenerator&) = default;

public:
	~BulletGenerator() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
		ThreadPool::bulletGenerator = gameObject;
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
	}

	int CreateBulletAndGetObjID(int type)
	{
		if (SceneManager::scenemanager->gameState == START) 
		{
			GameObject* object = Instantiate(bullet[type]);
			objIDmgr->GetComponent<ObjectIDManager>()->CreateObjectID(object);
		}
	}
	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

