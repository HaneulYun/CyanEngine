#pragma once
#include "framework.h"

class MovingBehavior : public Component
{
private:
	// �� ������ private ������ �����ϼ���.

public:
	// �� ������ public ������ �����ϼ���.
	Transform* target;

	float speed{ 15 };

private:
	friend class GameObject;
	MovingBehavior() {}
	MovingBehavior(const MovingBehavior&) = default;

public:
	~MovingBehavior() {}
	virtual Component* Duplicate() { return new MovingBehavior; };

	void Start()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
		target = new Transform();
		target->position = XMFLOAT3(0, 0, 0);
	}

	void Update()
	{
		// ������Ʈ �ڵ带 �ۼ��ϼ���.
		auto n = NS_Vector3::Normalize(NS_Vector3::Subtract(target->position, gameObject->transform->position));
		gameObject->transform->position = NS_Vector3::Add(gameObject->transform->position, n, speed * Time::deltaTime);
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

