#pragma once
#include "framework.h"

class MovingBehavior : public MonoBehavior<MovingBehavior>
{
private:
	// �� ������ private ������ �����ϼ���.
	float time{ 0.0f };

public:
	// �� ������ public ������ �����ϼ���.
	Vector3 direction;

	float speed{ 4.f };

private:
	friend class GameObject;
	friend class MonoBehavior<MovingBehavior>;
	MovingBehavior() = default;
	MovingBehavior(MovingBehavior&) = default;

public:
	~MovingBehavior() {}

	void Start()
	{
		time = 3.0f;
		direction = (Vector3(0, 0, 0) - gameObject->transform->position).Normalize();
	}

	void Update()
	{
		// ������Ʈ �ڵ带 �ۼ��ϼ���.
		gameObject->transform->position = gameObject->transform->position + direction * speed * Time::deltaTime;

		time -= Time::deltaTime;
		if(time < 0)
			gameObject->Destroy(gameObject);
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
	void OnTriggerEnter(GameObject* collision)
	{
		direction = Vector3(-direction.x, -direction.y, -direction.z);
	}
};

