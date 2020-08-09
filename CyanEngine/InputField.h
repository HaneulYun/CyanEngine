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
		Text();
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

		if (isFocused)
		{
			if (Input::buffer[0] || Input::isModifiedCbuffer)
			{
				int len = wcslen(Input::buffer);
				if (len && Input::buffer[len-1] == 13)
				{
					isFocused = false;
					return;
				}

				if (Input::GetKeyDown(KeyCode::Backspace) && text.size())
					text.erase(text.end() - 1);
				else
					text += Input::buffer;

				textComponent->text = text + Input::cbuffer;
			}
		}
	}

	Text* Text()
	{
		if (!textComponent)
		{
			auto child = gameObject->AddChildUI();
			auto rt = child->GetComponent<RectTransform>();
			rt->anchorMin = { 0, 0 };
			rt->anchorMax = { 1, 1 };

			textComponent = child->AddComponent<::Text>();
			textComponent->text = L"Input here";
		}
		return textComponent;
	}

	void setText(std::wstring str)
	{
		text = str;
		textComponent->text = text;
	}

	void clear()
	{
		text.clear();
		textComponent->text = text;
	}

	void setFocus(bool isFocus)
	{
		isFocused = isFocus;
	}
};