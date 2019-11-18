#pragma once
#include "framework.h"

class MovingBehavior : public MonoBehavior<MovingBehavior>
{
private:
	// 이 영역에 private 변수를 선언하세요.

public:
	// 이 영역에 public 변수를 선언하세요.
	Transform* target{ nullptr };

	float speed{ 90 };

	Vector3 old;

private:
	friend class GameObject;
	friend class MonoBehavior<MovingBehavior>;
	MovingBehavior() = default;
	MovingBehavior(MovingBehavior&) = default;

public:
	~MovingBehavior() {}
	//virtual Component* Duplicate() { return new MovingBehavior; }
	//virtual Component* Duplicate(Component* component) { return new MovingBehavior(*(MovingBehavior*)component); }

	void Start()
	{
		// 초기화 코드를 작성하세요.
		if (!target)
		{
			target = new Transform();
			target->position = Vector3(0, 0, 0);
		}

		old = (target->position - gameObject->transform->position).Normalize();
	}

	void Update()
	{
		// 업데이트 코드를 작성하세요.
		Vector3 v = (target->position - gameObject->transform->position).Normalize();

		auto t = NS_Vector3::Angle(v.xmf3, old.xmf3);
		if (t > 30)
		{
			gameObject->transform->position = Vector3{ 0, 0, 0 };
			speed = 0;
			Destroy(gameObject);
		}


		if (speed)
		{
			gameObject->transform->position = gameObject->transform->position + v * speed * Time::deltaTime;
		}

		old = v;
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

