#pragma once

struct PassLight
{
	Vector3 Strength{ 0.5f, 0.5f, 0.5f };
	float FalloffStart{ 1.0f };
	Vector3 Direction{ 0.0f, -1.0f, 0.0f };
	float FalloffEnd{ 10.0f };
	Vector3 Position{ 0.0f, 0.0f, 0.0f };
	float SpotPower{ 64.0f };
	int type{ 0 };
	int pad[3]{ 0 };
};

class Light : public MonoBehavior<Light>
{
public:
	enum Type { Directional, Point, Spot };
	Type type{ Directional };

	enum ShadowType { NoShadows, Shadows };// HardShadows, SoftShadows};
	ShadowType shadowType{ NoShadows };

	Vector3 Strength{ 0.5f, 0.5f, 0.5f };
	float FalloffStart{ 1.0f };
	//Vector3 Direction{ 0.0f, -1.0f, 0.0f };
	float FalloffEnd{ 10.0f };
	//Vector3 Position{ 0.0f, 0.0f, 0.0f };
	float SpotPower{ 64.0f };

protected:
	friend class GameObject;
	friend class MonoBehavior<Light>;
	Light() = default;
	Light(Light&) = default;

public:
	~Light() {}

	void Start();

	PassLight get(Vector3 look, Vector3 position) const
	{
		PassLight l{Strength,FalloffStart, look, FalloffEnd, position, SpotPower, (int)type };
		return l;
	}
};