#pragma once

class ComingBehavior : public MonoBehavior<ComingBehavior>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	float elapsedTime{ 0.f };
	float timeCycle{ 1.f };
	float boostTime{ 0.1f };
	bool boost{ false };
	float speed{ 3.f };

public  /*이 영역에 public 변수를 선언하세요.*/:
	Vector3 target;

private:
	friend class GameObject;
	friend class MonoBehavior<ComingBehavior>;
	ComingBehavior() = default;
	ComingBehavior(ComingBehavior&) = default;

public:
	~ComingBehavior() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
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

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

