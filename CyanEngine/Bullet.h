#pragma once
#include "framework.h"

class Enemy;

class Bullet : public MonoBehavior<Bullet>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	float timeCycle{ 0.5f };
	float speed{ 200.f };
	Vector3 direction;

private:
	friend class GameObject;
	friend class MonoBehavior<Bullet>;
	Bullet() = default;
	Bullet(Bullet&) = default;

public:
	~Bullet() {}

	void Start()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
		direction = direction.Normalize();
		gameObject->transform->Rotate({ 0, 0, 1 }, direction.Degree() - 90);
	}

	void Update()
	{
		Vector3 movevector = direction * speed * Time::deltaTime;
		gameObject->transform->position = gameObject->transform->position + movevector;
	}

	void OnTriggerEnter(GameObject* collision)
	{
		//if (!collision->GetComponent<Enemy>())
		//	return;
		//Destroy(gameObject);
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
	void SetEntity(const float _timeCycle, const float _speed, const Vector3& _direction = {})
	{
		timeCycle = _timeCycle;
		speed = _speed;
		direction = _direction;
	}
};