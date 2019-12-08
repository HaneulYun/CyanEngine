#pragma once
#include "framework.h"

class EmptyShell : public MonoBehavior<EmptyShell>
{
private /*�� ������ private ������ �����ϼ���.*/:
	float lifeTime{ 1.0f };
	float currentTime{ 1.0f };
	float maxSpeed{ 100.0f };
	Vector3 direction{};

public  /*�� ������ public ������ �����ϼ���.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<EmptyShell>;
	EmptyShell() = default;
	EmptyShell(EmptyShell&) = default;

public:
	~EmptyShell() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
		// EmptyShell �� SetEntity �� ���� �ʱⰪ�� �����Ͽ��� �մϴ�.
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		currentTime -= Time::deltaTime;
		float t = (currentTime / lifeTime);
		if (currentTime < 0.0f)
			Destroy(gameObject);

		float speed = maxSpeed * t * t;
		gameObject->transform->position += direction * speed * Time::deltaTime;
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
	void SetEntity(const Vector3& _direction, float _lifeTime)
	{
		currentTime = lifeTime = _lifeTime;
		direction = _direction;
	}
};