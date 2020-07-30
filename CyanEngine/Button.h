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

	void Start()
	{
	}

	void Update()
	{
		if (OnClick())
			for (auto& e : events)
				e.first(e.second);
	}

	bool OnClick()
	{
		if (gameObject->active && Input::GetMouseButtonUp(0))
			if (IsPointInRect(Camera::main->ScreenToViewportPoint(Input::mousePosition)))
				return true;
		return false;
	}

	bool IsPointInRect(Vector3 position)
	{
		RectTransform* rect{ nullptr };
		if (rect = gameObject->GetComponent<RectTransform>(); !rect)
			return false;

		Matrix4x4 mat = RectTransform::Transform(gameObject->GetMatrix());;

		Vector3 leftTop{ mat._41, mat._22 + mat._42, 0 };
		Vector3 rightBottom{ mat._11 + mat._41, mat._42, 0 };

		if (position.x < leftTop.x || position.x > rightBottom.x ||
			position.y < rightBottom.y || position.y > leftTop.y)
			return false;
		return true;
	}

	void AddEvent(void(*func)(void*), void* data = nullptr)
	{
		events.emplace_back(std::make_pair(func, data));
	}
};