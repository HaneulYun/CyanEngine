#pragma once
#include "framework.h"

class MovingBehavior : public MonoBehavior<MovingBehavior>
{
private:
	// 이 영역에 private 변수를 선언하세요.

public:
	// 이 영역에 public 변수를 선언하세요.
	Vector3 direction;

	float speed{ 100.f };

private:
	friend class GameObject;
	friend class MonoBehavior<MovingBehavior>;
	MovingBehavior() = default;
	MovingBehavior(MovingBehavior&) = default;

public:
	~MovingBehavior() {}

	void Start()
	{
		// 초기화 코드를 작성하세요.
		direction = (Vector3(0, 100, 0) - gameObject->transform->position).Normalize();
	}

	void Update()
	{
		// 업데이트 코드를 작성하세요.
		gameObject->transform->position = gameObject->transform->position + direction * speed * Time::deltaTime;
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
	void OnTriggerEnter(GameObject* collision)
	{
		direction = Vector3(-direction.x, -direction.y, -direction.z);
	}
};

