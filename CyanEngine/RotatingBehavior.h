#pragma once
#include "framework.h"

class RotatingBehavior : public Component
{
public:
	XMFLOAT3 axis{ 0.0f, 1.0f, 0.0f };
	float speedRotating{ 90.f };

private:
	friend class GameObject;
	RotatingBehavior() {}

public:
	~RotatingBehavior() {}

	void Start() 
	{
	}

	void Update() 
	{
		gameObject->transform ->Rotate(axis, speedRotating * Time::deltaTime);
	}

	virtual Component* Duplicate() { return new RotatingBehavior; };
};

