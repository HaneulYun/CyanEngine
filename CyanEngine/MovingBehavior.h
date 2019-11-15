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
	MovingBehavior(MovingBehavior&) = default;

public:
	~MovingBehavior() {}
	virtual Component* Duplicate() { return new MovingBehavior; }
	virtual Component* Duplicate(Component * component) { return new MovingBehavior(*(MovingBehavior*)component); }

	void Start()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
	}

	void Update()
	{
		// ������Ʈ �ڵ带 �ۼ��ϼ���.
		auto n = NS_Vector3::Normalize(NS_Vector3::Subtract(target->position.xmf3, gameObject->transform->position.xmf3));
		gameObject->transform->position.xmf3 = NS_Vector3::Add(gameObject->transform->position.xmf3, n, speed * Time::deltaTime);
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

