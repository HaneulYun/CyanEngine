#pragma once
#include "framework.h"

class RotatingBehavior : public MonoBehavior<RotatingBehavior>
{
public:
	XMFLOAT3 axis{ 0.0f, 1.0f, 0.0f };
	float speedRotating{ 0.0f };

private:
	friend class GameObject;
	friend class MonoBehavior<RotatingBehavior>;
	RotatingBehavior() = default;
	RotatingBehavior(RotatingBehavior&) = default;

public:
	~RotatingBehavior() {}

	void Start() 
	{
		if (speedRotating == 0.0f)
			speedRotating = Random::Range(-90.0f, 90.0f);
	}

	void Update() 
	{
		gameObject->transform->Rotate(axis, speedRotating * Time::deltaTime);
	}
};

