#pragma once
#include "framework.h"

class RevolvingBehavior : public Component
{
private:
	XMFLOAT3 axis{ 0.0f, 0.0f, 1.0f };

public:
	GameObject* target{ nullptr };
	float radius{ 10.f };
	float speedRotating{ 30.f };
	float angle{ 0.0f };

private:
	friend class GameObject;
	RevolvingBehavior() {}

public:
	~RevolvingBehavior() {}

	void Start()
	{
	}

	void Update()
	{
		angle += speedRotating * Time::deltaTime;
		gameObject->transform->position = Vector3::Add(target->transform->position,
			XMFLOAT3(radius * cos(angle * PI / 180.0f), radius * sin(angle * PI / 180.0f), 0.0f));
	}

	virtual Component* Duplicate() { return new RevolvingBehavior; };
};