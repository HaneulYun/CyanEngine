#pragma once
#include "framework.h"

class Bullet : public Component
{
private:
	// 이 영역에 private 변수를 선언하세요

public:
	// 이 영역에 public 변수를 선언하세요.
	float timeCycle{ 0.5f };
	float speed{ 200.f };
	Vector3 direction;

private:
	friend class GameObject;
	Bullet() {}
	Bullet(Bullet&) = default;

public:
	~Bullet() {}
	virtual Component* Duplicate() { return new Bullet; };
	virtual Component* Duplicate(Component* component) { return new Bullet(*(Bullet*)component); }

	void Start()
	{
		// 초기화 코드를 작성하세요.
		direction = direction.Normalize();
	}

	void Update()
	{
		Vector3 movevector = direction * speed * Time::deltaTime;
		gameObject->transform->position = gameObject->transform->position + movevector;
	}



	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};