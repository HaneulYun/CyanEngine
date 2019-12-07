#pragma once
#include "framework.h"

class ComingBehavior : public MonoBehavior<ComingBehavior>
{
private /*�� ������ private ������ �����ϼ���.*/:
	float speed{ 100.f };
	float maxSpeed{ 100.f };

public  /*�� ������ public ������ �����ϼ���.*/:
	Vector3 target;

private:
	friend class GameObject;
	friend class MonoBehavior<ComingBehavior>;
	ComingBehavior() = default;
	ComingBehavior(ComingBehavior&) = default;

public:
	~ComingBehavior() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		Vector3 dir = (target - gameObject->transform->position).Normalize();

		gameObject->transform->position += dir * speed * Time::deltaTime;

		speed -= Time::deltaTime * maxSpeed * 1.5f;

		if (speed <= 0.f)
		{
			speed = maxSpeed;
		}

	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

