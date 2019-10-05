#pragma once

class RotatingBehavior : public Component
{
private:
	XMFLOAT3 axis{ 0.0f, 1.0f, 0.0f };

public:
	float speedRotating{ 90.f };

public:
	RotatingBehavior();
	~RotatingBehavior();

	virtual void Start();

	virtual void Update();

	virtual void Render();

	virtual void Destroy();
};

