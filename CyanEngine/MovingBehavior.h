#pragma once
#include "framework.h"

class MovingBehavior : public MonoBehavior<MovingBehavior>
{
private:
	// �� ������ private ������ �����ϼ���.

public:
	// �� ������ public ������ �����ϼ���.
	Vector3 direction;

	float speed{ 100.f };

private:
	friend class GameObject;
	friend class MonoBehavior<MovingBehavior>;
	MovingBehavior() = default;
	MovingBehavior(MovingBehavior&) = default;

public:
	~MovingBehavior() {}

	void Start()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
		direction = (Vector3(0, 100, 0) - gameObject->transform->position).Normalize();
	}

	void Update()
	{
		// ������Ʈ �ڵ带 �ۼ��ϼ���.
		gameObject->transform->position = gameObject->transform->position + direction * speed * Time::deltaTime;
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
	void OnTriggerEnter(GameObject* collision)
	{
		direction = Vector3(-direction.x, -direction.y, -direction.z);
	}
};

