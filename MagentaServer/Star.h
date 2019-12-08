#pragma once

class Star : public Component
{
private:
	friend class GameObject;
	Star() {}

public:
	~Star() {}
	virtual Component* Duplicate() { return new Star; };

	void Start()
	{
		// 초기화 코드를 작성하세요.
	}

	void Update()
	{
		
	}

	void OnTriggerEnter(GameObject* collision)
	{
		if (collision->GetComponent<Enemy>() != NULL)
		{
			int enemyID = collision->GetComponent<ObjectID>()->GetObjectID();
			Message sendMsg;
			sendMsg.msgId = MESSAGE_NOTIFY_COLLISION_STAR_AND_ENEMY;
			sendMsg.lParam = enemyID;
			sendMsg.mParam = gameObject->GetComponent<Damageable>()->GetCurHealth();
			SceneManager::scenemanager->objectIDmanager->GetComponent<ObjectIDManager>()->DeleteObjectID(enemyID);
			printf("삭제 %d\n", enemyID);
			
			EnterCriticalSection(&ThreadPool::sqcs);
			ThreadPool::sendQueue.push(sendMsg);
			LeaveCriticalSection(&ThreadPool::sqcs);
		}
	}
};