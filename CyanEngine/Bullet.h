#pragma once
#include "framework.h"

class Bullet : public Component
{
private:
	// �� ������ private ������ �����ϼ���
public:
	// �� ������ public ������ �����ϼ���.
	float timeCycle{ 0.5f };
	float speed{ 200.f };
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

		// Delete Bullet Out of Range
		Vector3 distance = gameObject->transform->position - Vector3{ 0.0f,0.0f,0.0f };
		if (distance.Length() >= 300.0f)
		{
			Destroy(gameObject);
		}
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
	void SetEntity(const float _timeCycle, const float _speed, const Vector3& _direction = {})
	{
		timeCycle = _timeCycle;
		speed = _speed;
		direction = _direction;
	}
};