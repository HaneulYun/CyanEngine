#pragma once

class Text : public MonoBehavior<Text>
{
public:
	std::wstring text{ L"New Text" };
	std::wstring font{ L"µ¸À½" };
	int fontSize{ 14 };
	Vector4 color{ 0.0f, 0.0f, 0.0f, 1.0f };

	DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment{ DWRITE_PARAGRAPH_ALIGNMENT_NEAR };
	DWRITE_TEXT_ALIGNMENT textAlignment{ DWRITE_TEXT_ALIGNMENT_LEADING };
	DWRITE_FONT_STYLE style{ DWRITE_FONT_STYLE_NORMAL };
	DWRITE_FONT_WEIGHT fontWeight{ DWRITE_FONT_WEIGHT_NORMAL };

	int brushIndex{ -1 };
	int formatIndex{ -1 };

private:
	friend class GameObject;
	friend class MonoBehavior<Text>;
	Text() = default;
	Text(Text&) = default;

public:
	~Text() {}

	void Start();
	void Update() {}
	void OnDestroy();
};