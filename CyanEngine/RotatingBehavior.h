#pragma once
#include "framework.h"

class RotatingBehavior : public MonoBehavior<RotatingBehavior>
{
public:
	XMFLOAT3 axis{ 0.0f, 0.0f, 1.0f };
	float speedRotating{ 90.f };

private:
	friend class GameObject;
	friend class MonoBehavior<RotatingBehavior>;
	RotatingBehavior() = default;
	RotatingBehavior(RotatingBehavior&) = default;

public:
	~RotatingBehavior() {}

	void Start() 
	{
	}

	void Update() 
	{
		gameObject->transform ->Rotate(axis, speedRotating * Time::deltaTime);
	}
};

