#pragma once
#include "framework.h"

class MovingBehavior : public MonoBehavior<MovingBehavior>
{
private:
	// �� ������ private ������ �����ϼ���.

public:
	// �� ������ public ������ �����ϼ���.
	Transform* target{ nullptr };

	float speed{ 90 };

	Vector3 old;

private:
	friend class GameObject;
	friend class MonoBehavior<MovingBehavior>;
	MovingBehavior() = default;
	MovingBehavior(MovingBehavior&) = default;

public:
	~MovingBehavior() {}
	//virtual Component* Duplicate() { return new MovingBehavior; }
	//virtual Component* Duplicate(Component* component) { return new MovingBehavior(*(MovingBehavior*)component); }

	void Start()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
		if (!target)
		{
			target = new Transform();
			target->position = Vector3(0, 0, 0);
		}

		old = (target->position - gameObject->transform->position).Normalize();
	}

	void Update()
	{
		// ������Ʈ �ڵ带 �ۼ��ϼ���.
		Vector3 v = (target->position - gameObject->transform->position).Normalize();

		auto t = NS_Vector3::Angle(v.xmf3, old.xmf3);
		if (t > 30)
		{
			gameObject->transform->position = Vector3{ 0, 0, 0 };
			speed = 0;
			Destroy(gameObject);
		}


		if (speed)
		{
			gameObject->transform->position = gameObject->transform->position + v * speed * Time::deltaTime;
		}

		old = v;
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

