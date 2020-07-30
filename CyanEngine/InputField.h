#pragma once

class InputField : public MonoBehavior<InputField>
{
public:
	Button* button{ nullptr };
	Text* textComponent{ nullptr };
	

	bool isFocused{ false };

	std::wstring text;

private:
	friend class GameObject;
	friend class MonoBehavior<InputField>;
	InputField() = default;
	InputField(InputField&) = default;

public:
	~InputField() {}

	void Start()
	{
		button = gameObject->AddComponent<Button>();

		auto child = gameObject->AddChildUI();
		auto rt = child->GetComponent<RectTransform>();
		rt->anchorMin = { 0, 0 };
		rt->anchorMax = { 1, 1 };

		textComponent = child->AddComponent<Text>();
		textComponent->text = L"Input here";
	}
	void Update()
	{
		if (Input::GetMouseButtonUp(0))
		{
			if (button->OnClick())
				isFocused = true;
			else
				isFocused = false;
		}

		if(isFocused)
			if (Input::buffer[0])
			{
				if (Input::buffer[0] == 13)
				{
					isFocused = false;
				}
				else
				{
					text += Input::buffer;
					textComponent->text = text + Input::cbuffer;
				}
			}
	}
};