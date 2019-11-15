#pragma once
#include "framework.h"

class Spawner : public Component
{
private:
	// 이 영역에 private 변수를 선언하세요.
	float spawnTime{ 0.3 };
	float elapsedTime{ 0 };

	float spawnRadius{ 40 };

public:
	// 이 영역에 public 변수를 선언하세요.
	GameObject* enemy;

private:
	friend class GameObject;
	Spawner() {}

public:
	~Spawner() {}
	virtual Component* Duplicate() { return new Spawner; };

	void Start()
	{
		// 초기화 코드를 작성하세요.
	}

	void Update()
	{
		// 업데이트 코드를 작성하세요.
		elapsedTime += Time::deltaTime;

		if (elapsedTime > spawnTime)
		{
			elapsedTime -= spawnTime;

			GameObject* object = Instantiate(enemy);
			{
				float radian = Random::Range(0.0f, XM_2PI);
				object->GetComponent<Transform>()->position = Vector3(cos(radian) * spawnRadius, sin(radian) * spawnRadius, 0);
			}
		}
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

