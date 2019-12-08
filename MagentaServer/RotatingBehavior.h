#pragma once

class RotatingBehavior : public MonoBehavior<RotatingBehavior>
{
private:
	XMFLOAT3 axis{ 0.0f, 0.0f, 1.0f };

public:
	XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
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
		gameObject->transform->Rotate(axis, speedRotating * Time::deltaTime);
	}
};