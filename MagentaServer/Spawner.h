#pragma once

class Spawner : public Component
{
private:
	// 이 영역에 private 변수를 선언하세요.
	float startTime{ 0.0 };
	float spawnTime{ 0.5 };
	float elapsedTime{ 0 };

	float spawnRadius{ 200 };
	int level{ 0 };
	
public:
	// 이 영역에 public 변수를 선언하세요.
	GameObject* enemy[5]{ nullptr };
	ObjectIDManager* objIDmgr{ nullptr };

private:
	friend class GameObject;
	Spawner() {}
	Spawner(Spawner&) = default;

public:
	~Spawner() {}
	virtual Component* Duplicate() { return new Spawner; }
	virtual Component* Duplicate(Component* component) { return new Spawner(*(Spawner*)component); }

	void Start()
	{
		// 초기화 코드를 작성하세요.
	}

	void Update()
	{
		// 업데이트 코드를 작성하세요.
		if (SceneManager::scenemanager->gameState == START) {

			startTime += Time::deltaTime;
			elapsedTime += Time::deltaTime;

			if (level == 0 && startTime > 10.f)
				level = 1;
			else if (level == 1 && startTime > 20.f)
				level = 2;
			else if (level == 2 && startTime > 30.f)
				level = 4;

			if (elapsedTime > spawnTime)
			{
				elapsedTime -= spawnTime;

				int Rand = Random::Range(0, 100);
				int enemyNum;
				if (level == 0)
					enemyNum = 0;
				else if (level == 1)
				{
					if (Rand >= 70)
						enemyNum = 1;
					else
						enemyNum = 0;
				}
				else if (level == 2)
				{
					if (Rand >= 90)
						enemyNum = 2;
					else if (Rand >= 65)
						enemyNum = 1;
					else
						enemyNum = 0;
				}
				else
				{
					if (Rand >= 90)
						enemyNum = 2;
					else if (Rand >= 70)
						enemyNum = 1;
					else if (Rand >= 50)
						enemyNum = 4;
					else
						enemyNum = 0;
				}

				GameObject* object = Instantiate(enemy[enemyNum]);
				{
					float radian = Random::Range(0.0f, XM_2PI);
					object->GetComponent<Transform>()->position = Vector3(cos(radian) * spawnRadius, sin(radian) * spawnRadius, 0);
					objIDmgr->CreateObjectID(object);

					Message result;

					if (enemyNum == 0)
						result.msgId = MESSAGE_CREATE_ENEMY_COMINGRECT;
					else if (enemyNum == 1)
						result.msgId = MESSAGE_CREATE_ENEMY_ROTATINGRECT;
					else if (enemyNum == 2)
						result.msgId = MESSAGE_CREATE_ENEMY_COMINGBIGRECT;
					else if (enemyNum == 4)
						result.msgId = MESSAGE_CREATE_ENEMY_WHIRLINGCOMINGRECT;

					result.lParam = object->GetComponent<ObjectID>()->GetObjectID();
					result.rParam = radian;

					EnterCriticalSection(&ThreadPool::sqcs);
					ThreadPool::sendQueue.push(result);
					LeaveCriticalSection(&ThreadPool::sqcs);
				}
			}
		}
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

