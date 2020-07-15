#pragma once

struct PassLight
{
	Vector3 Strength{ 0.5f, 0.5f, 0.5f };
	float FalloffStart{ 1.0f };
	Vector3 Direction{ 0.0f, -1.0f, 0.0f };
	float FalloffEnd{ 10.0f };
	Vector3 Position{ 0.0f, 0.0f, 0.0f };
	float SpotPower{ 64.0f };
};

class Light : public MonoBehavior<Light>
{
private:

public:
	Vector3 Strength{ 0.5f, 0.5f, 0.5f };
	float FalloffStart{ 1.0f };
	Vector3 Direction{ 0.0f, -1.0f, 0.0f };
	float FalloffEnd{ 10.0f };
	Vector3 Position{ 0.0f, 0.0f, 0.0f };
	float SpotPower{ 64.0f };

protected:
	friend class GameObject;
	friend class MonoBehavior<Light>;
	Light(Light&) = default;

public:
	Light() = default;
	~Light() {}

	void Start();
};