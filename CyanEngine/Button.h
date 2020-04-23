#pragma once

class Button : public MonoBehavior<Button>
{
private:

public:

private:
	friend class GameObject;
	friend class MonoBehavior<Button>;
	Button() = default;
	Button(Button&) = default;

public:
	~Button() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void OnClick()
	{
	}
};