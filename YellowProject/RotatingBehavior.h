#pragma once
#include "..\CyanEngine\framework.h"

class RotatingBehavior : public MonoBehavior<RotatingBehavior>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	Vector3 axis{ 0, 1, 0 };
	float speed{ 90 };

private:
	friend class GameObject;
	friend class MonoBehavior<RotatingBehavior>;
	RotatingBehavior() = default;
	RotatingBehavior(RotatingBehavior&) = default;

public:
	~RotatingBehavior() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		gameObject->transform->Rotate(axis, speed * Time::deltaTime);
		gameObject->transform->position = gameObject->transform->forward * -500;
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
	void setAxisAndSpeed(Vector3 axis, float speed)
	{
		this->axis = axis;
		this->speed = speed;
	}
};