#pragma once
#include "framework.h"

class Bullet : public Component
{
private:
	// �� ������ private ������ �����ϼ���.
	float speed{ 50.f };

public:
	// �� ������ public ������ �����ϼ���.
	Vector3 direction;

private:
	friend class GameObject;
	Bullet() {}

public:
	~Bullet() {}
	virtual Component* Duplicate() { return new Bullet; };

	void Start()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
		direction = NS_Vector3::Normalize(direction);
	}

	void Update()
	{
		Vector3 movevector = NS_Vector3::ScalarProduct(direction, speed * Time::deltaTime);
		gameObject->transform->position = NS_Vector3::Add(gameObject->transform->position, movevector);
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};