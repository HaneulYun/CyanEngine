#pragma once
#include "framework.h"

class Bullet : public Component
{
private:
	// 이 영역에 private 변수를 선언하세요.
	float speed{ 50.f };

public:
	// 이 영역에 public 변수를 선언하세요.
	Vector3 direction;

private:
	friend class GameObject;
	Bullet() {}

public:
	~Bullet() {}
	virtual Component* Duplicate() { return new Bullet; };

	void Start()
	{
		// 초기화 코드를 작성하세요.
		direction = NS_Vector3::Normalize(direction);
	}

	void Update()
	{
		Vector3 movevector = NS_Vector3::ScalarProduct(direction, speed * Time::deltaTime);
		gameObject->transform->position = NS_Vector3::Add(gameObject->transform->position, movevector);
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};