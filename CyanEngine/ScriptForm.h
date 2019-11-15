#pragma once
#include "framework.h"

class Bullet : public Component
{
private:
	// 이 영역에 private 변수를 선언하세요.

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
	}

	void Update()
	{
		gameObject->transform->position = NS_Vector3::Add
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

