#pragma once

class InputField : public MonoBehavior<InputField>
{
public:
	Text* textComponent{ nullptr };

	bool isFocused{ true };

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
		gameObject->AddComponent<Button>()->AddEvent([](void*) { });

		auto child = gameObject->AddChildUI();
		auto rt = child->GetComponent<RectTransform>();
		rt->anchorMin = { 0, 0 };
		rt->anchorMax = { 1, 1 };

		textComponent = child->AddComponent<Text>();
		textComponent->text = L"Input here";
	}
	void Update()
	{
		if (Input::buffer[0])
		{
			text += Input::buffer;
			textComponent->text = text + Input::cbuffer;
		}
	}
};