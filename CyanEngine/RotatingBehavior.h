#pragma once
#include "framework.h"

class RotatingBehavior : public Component
{
private:
	XMFLOAT3 axis{ 0.0f, 1.0f, 0.0f };

public:
	XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
	float speedRotating{ 90.f };

private:
	friend class GameObject;
	RotatingBehavior() {}

public:
	~RotatingBehavior() {}

	void Start() 
	{
		gameObject->transform->position = pos;
	}

	void Update() 
	{
		gameObject->transform->Rotate(axis, speedRotating * Time::Instance()->GetTimeElapsed());
	}
};

