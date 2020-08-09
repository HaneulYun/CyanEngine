#pragma once

class OnUI : public MonoBehavior<OnUI>
{
public:
	Vector2 leftTop{};
	Vector2 rightBottom{};

private:
	friend class GameObject;
	friend class MonoBehavior<OnUI>;
	OnUI() = default;
	OnUI(OnUI&) = default;

public:
	~OnUI() {}

	void Start()
	{
	}

	void Update()
	{
	}
};