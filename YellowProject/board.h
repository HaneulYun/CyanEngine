#pragma once
#include "framework.h"

class Board : public MonoBehavior<Board>
{
private:
	Vector3 axis{ 0.0f, 0.0f, 1.0f };

public:
	Vector3 pos{ 0.0f, 0.0f, 0.0f };
	float speedRotating{ 90.f };

private:
	friend class GameObject;
	friend class MonoBehavior<Board>;
	Board() = default;
	Board(Board&) = default;

public:
	~Board() {}

	void Start()
	{
	}

	void Update()
	{
		gameObject->transform->Rotate(axis, speedRotating * Time::deltaTime);
	}
};
