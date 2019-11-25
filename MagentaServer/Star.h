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
		if (collision->GetComponent<MovingBehavior>() != NULL)
		{
			if (!collision->GetComponent<MovingBehavior>()->collision)
			{
				collision->GetComponent<MovingBehavior>()->collision = true;
				Message sendMsg;
				sendMsg.msgId = MESSAGE_NOTIFY_COLLISION_STAR_AND_ENEMY;
				sendMsg.mParam = gameObject->GetComponent<Damageable>()->GetCurHealth();
				EnterCriticalSection(&ThreadPool::sqcs);
				ThreadPool::sendQueue.push(sendMsg);
				LeaveCriticalSection(&ThreadPool::sqcs);
			}
		}
	}
};