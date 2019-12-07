#pragma once
#include "framework.h"

class ComingBehavior : public MonoBehavior<ComingBehavior>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	float speed{ 100.f };
	float maxSpeed{ 100.f };

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

		speed -= Time::deltaTime * maxSpeed * 1.5f;

		if (speed <= 0.f)
		{
			speed = maxSpeed;
		}

	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

