#pragma once
#include "framework.h"

class Spawner : public Component
{
private:
	// �� ������ private ������ �����ϼ���.
	float spawnTime{ 0.3 };
	float elapsedTime{ 0 };

	float spawnRadius{ 40 };

public:
	// �� ������ public ������ �����ϼ���.
	GameObject* enemy;

private:
	friend class GameObject;
	Spawner() {}

public:
	~Spawner() {}
	virtual Component* Duplicate() { return new Spawner; };

	void Start()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
	}

	void Update()
	{
		// ������Ʈ �ڵ带 �ۼ��ϼ���.
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

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

