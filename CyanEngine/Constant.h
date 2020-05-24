#pragma once

class Constant : public MonoBehavior<Constant>
{
public:
	Vector4 v4;
private:
	friend class GameObject;
	friend class MonoBehavior<Constant>;
	Constant() = default;
	Constant(Constant&) = default;

public:
	~Constant() {}

	void Start()
	{
	}

	void Update()
	{
	}
};