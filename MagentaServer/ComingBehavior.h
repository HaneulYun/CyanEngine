#pragma once

class ComingBehavior : public MonoBehavior<ComingBehavior>
{
private /*�� ������ private ������ �����ϼ���.*/:
	float elapsedTime{ 0.f };
	float timeCycle{ 1.f };
	float boostTime{ 0.1f };
	bool boost{ false };
	float speed{ 3.f };

public  /*�� ������ public ������ �����ϼ���.*/:
	Vector3 target;

private:
	friend class GameObject;
	friend class MonoBehavior<ComingBehavior>;
	ComingBehavior() = default;
	ComingBehavior(ComingBehavior&) = default;

public:
	~ComingBehavior() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		Vector3 dir = (target - gameObject->transform->position).Normalize();

		gameObject->transform->position += dir * speed * Time::deltaTime;

		if (boost && elapsedTime > boostTime)
		{
			elapsedTime -= boostTime;
			boost = false;
			speed = 10.f;
		}
		else if (!boost && elapsedTime > timeCycle)
		{
			elapsedTime -= timeCycle;
			boost = true;
			speed = 100.f;
		}

		elapsedTime += Time::deltaTime;
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

