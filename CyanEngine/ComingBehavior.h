#pragma once
#include "framework.h"

class ComingBehavior : public MonoBehavior<ComingBehavior>
{
private /*�� ������ private ������ �����ϼ���.*/:
	float speed{ 100.f };
	float maxSpeed{ 30.f };
	
	float creationCycle{ 1.0f };
	float currentCycle{ 0.0f };

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
		currentCycle -= Time::deltaTime;
		if (currentCycle < 0.0f)
			currentCycle += creationCycle;
		float t = (currentCycle / creationCycle);

		Vector3 dir = (target - gameObject->transform->position).Normalize();
		gameObject->transform->position += dir * t * maxSpeed * Time::deltaTime;

		//if (speed <= 0.f)
		//{
		//	speed = maxSpeed;
		//}
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

