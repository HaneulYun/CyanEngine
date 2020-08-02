#pragma once

class Button : public MonoBehavior<Button>
{
public:
	std::vector<std::pair<void(*)(void*), void*>> events;

private:
	friend class GameObject;
	friend class MonoBehavior<Button>;
	Button() = default;
	Button(Button&) = default;

public:
	~Button() {}

	void Start();
	void Update();

	bool OnClick();

	bool IsPointInRect(Vector3 position);

	void AddEvent(void(*func)(void*), void* data = nullptr);
};