#pragma once
#include "..\CyanEngine\framework.h"

class RotatingBehavior : public MonoBehavior<RotatingBehavior>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	Vector3 axis{ 0, 1, 0 };
	float speed{ 90 };

private:
	friend class GameObject;
	friend class MonoBehavior<RotatingBehavior>;
	RotatingBehavior() = default;
	RotatingBehavior(RotatingBehavior&) = default;

public:
	~RotatingBehavior() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		gameObject->transform->Rotate(axis, speed * Time::deltaTime);
		gameObject->transform->position = gameObject->transform->forward * -500;
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
	void setAxisAndSpeed(Vector3 axis, float speed)
	{
		this->axis = axis;
		this->speed = speed;
	}
};