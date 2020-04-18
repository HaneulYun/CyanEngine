#pragma once

class Text : public MonoBehavior<Text>
{
public:
	std::wstring text;
	XMFLOAT4 textBox;

	std::wstring font;
	int fontSize;
	D2D1_COLOR_F color;
	int paragraphAlignment;
	int alignment;
	int weight;
	int style;

	int brushIndex = -1;
	int formatIndex = -1;

private:
	friend class GameObject;
	friend class MonoBehavior<Text>;
	Text() = default;
	Text(Text&) = default;

public:
	~Text() {}

	void Start() {}
	void Update() {}

	void InitFontFormat(std::wstring font = L"µ¸¿ò", XMFLOAT4 textBox = { 0,0,1,1 }, int fontSize = 20, D2D1_COLOR_F color = { 1,1,1,1 },
		int paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR, int alignment = DWRITE_TEXT_ALIGNMENT_LEADING,
		int weight = DWRITE_FONT_WEIGHT_NORMAL, int style = DWRITE_FONT_STYLE_NORMAL)
	{
		this->font = font;
		this->textBox = textBox;
		this->fontSize = fontSize;
		this->color = color;
		this->paragraphAlignment = paragraphAlignment;
		this->alignment = alignment;
		this->weight = weight;
		this->style = style;

		brushIndex = -1;
		formatIndex = -1;
	}

	void SetAlignmentLeftTop() 
	{ 
		paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR; 
		alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
		formatIndex = -1;
	}
	void SetAlignmentCenter() 
	{ 
		paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
		alignment = DWRITE_TEXT_ALIGNMENT_CENTER;
		formatIndex = -1;
	}
	void SetFontStyleBold()
	{
		weight = DWRITE_FONT_WEIGHT_BOLD;
		formatIndex = -1;
	}
	void SetFontStyleNormal()
	{
		weight = DWRITE_FONT_WEIGHT_NORMAL;
		formatIndex = -1;
	}
};