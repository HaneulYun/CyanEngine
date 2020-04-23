#pragma once

struct FuncData
{
	void(*func)(void*);
	void* data;
	
	static FuncData Make(void* data, void(*func)(void*))
	{
		return { func, data };
	}
};

class Button : public MonoBehavior<Button>
{
public:
	std::vector<FuncData> funcData;

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
		if (Input::GetMouseButtonDown(0))
		{
			Vector3 screen = Camera::main->ScreenToViewportPoint(Input::mousePosition);
			if (!IsPointInRect(screen))
				return;
			for (auto& v : funcData)
				v.func(v.data);
		}
	}

	bool IsPointInRect(Vector3 position)
	{
		RectTransform* rect{ nullptr };
		if (rect = gameObject->GetComponent<RectTransform>(); !rect)
			return false;

		XMFLOAT4X4 mat = rect->localToWorldMatrix;

		Vector3 leftTop{ mat._41, mat._22 + mat._42, 0 };
		Vector3 rightBottom{ mat._11 + mat._41, mat._42, 0 };

		if (position.x < leftTop.x || position.x > rightBottom.x ||
			position.y < rightBottom.y || position.y > leftTop.y)
			return false;
		return true;
	}
};