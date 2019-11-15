#pragma once
#include "framework.h"

class MovingBehavior : public Component
{
private:
	// 이 영역에 private 변수를 선언하세요.

public:
	// 이 영역에 public 변수를 선언하세요.
	Transform* target;

	float speed{ 15 };

private:
	friend class GameObject;
	MovingBehavior() {}
	MovingBehavior(MovingBehavior&) = default;

public:
	~MovingBehavior() {}
	virtual Component* Duplicate() { return new MovingBehavior; }
	virtual Component* Duplicate(Component * component) { return new MovingBehavior(*(MovingBehavior*)component); }

	void Start()
	{
		// 초기화 코드를 작성하세요.
	}

	void Update()
	{
		// 업데이트 코드를 작성하세요.
		auto n = NS_Vector3::Normalize(NS_Vector3::Subtract(target->position.xmf3, gameObject->transform->position.xmf3));
		gameObject->transform->position.xmf3 = NS_Vector3::Add(gameObject->transform->position.xmf3, n, speed * Time::deltaTime);
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

