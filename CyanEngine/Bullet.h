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
	Bullet(Bullet&) = default;

public:
	~Bullet() {}
	virtual Component* Duplicate() { return new Bullet; };
	virtual Component* Duplicate(Component* component) { return new Bullet(*(Bullet*)component); }

	void Start()
	{
		// 초기화 코드를 작성하세요.
		direction.xmf3 = NS_Vector3::Normalize(direction.xmf3);
	}

	void Update()
	{
		Vector3 movevector( NS_Vector3::ScalarProduct(direction.xmf3, speed * Time::deltaTime) );
		gameObject->transform->position.xmf3 = NS_Vector3::Add(gameObject->transform->position.xmf3, movevector.xmf3);
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};