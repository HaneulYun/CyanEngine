#pragma once
#include "pch.h"

class Bullet : public Component
{
private:
	// �� ������ private ������ �����ϼ���.
	float speed{ 200.f };

public:
	// �� ������ public ������ �����ϼ���.
	Vector3 direction;

private:
	friend class GameObject;
	Bullet() {}
	Bullet(Bullet&) = default;

public:
	~Bullet() {}
	virtual Component* Duplicate() { return new Bullet; };
	virtual Component* Duplicate(Component* component) { return new Bullet(*(Bullet*)component); }

	void Start()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
		direction = direction.Normalize();
	}

	void Update()
	{
		Vector3 movevector = direction * speed * Time::deltaTime;
		gameObject->transform->position = gameObject->transform->position + movevector;
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};