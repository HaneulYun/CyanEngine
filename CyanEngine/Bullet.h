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
	Bullet(Bullet&) = default;

public:
	~Bullet() {}
	virtual Component* Duplicate() { return new Bullet; };
	virtual Component* Duplicate(Component* component) { return new Bullet(*(Bullet*)component); }

	void Start()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
		direction.xmf3 = NS_Vector3::Normalize(direction.xmf3);
	}

	void Update()
	{
		Vector3 movevector( NS_Vector3::ScalarProduct(direction.xmf3, speed * Time::deltaTime) );
		gameObject->transform->position.xmf3 = NS_Vector3::Add(gameObject->transform->position.xmf3, movevector.xmf3);
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};