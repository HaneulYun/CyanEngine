#pragma once
#include "framework.h"

class RotatingBehavior : public Component
{
private:
	XMFLOAT3 axis{ 0.0f, 0.0f, 1.0f };

public:
	XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
	float speedRotating{ 90.f };

private:
	friend class GameObject;
	RotatingBehavior() {}
	RotatingBehavior(RotatingBehavior&) = default;

public:
	~RotatingBehavior() {}
	virtual Component* Duplicate() { return new RotatingBehavior; }
	virtual Component* Duplicate(Component* component) { return new RotatingBehavior(*(RotatingBehavior*)component); }

	void Start() 
	{
	}

	void Update() 
	{
		gameObject->transform ->Rotate(axis, speedRotating * Time::deltaTime);

		gameObject->transform->position = Camera::main->ScreenToWorldPoint(Input::mousePosition);
	}

};

