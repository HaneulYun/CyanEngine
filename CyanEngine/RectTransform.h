#pragma once

struct Vector2
{
	float x;
	float y;
};

struct ImageInitializer
{
	float pad0, pad1, apd2, pad3, pad4;
};

class RectTransform : public MonoBehavior<RectTransform, Transform>
{
public:
	union
	{
		ImageInitializer pad{ 0, 0, 100, 100, 0 };
		struct
		{
			float posX;
			float posY;
			float width;
			float height;
			float posZ;
		};
		struct
		{

			float left;
			float top;
			float right;
			float bottom;
		};
	};
	Vector2 anchorMin{ 0.5f, 0.5f };
	Vector2 anchorMax{ 0.5f, 0.5f };
	Vector2 pivot{ 0.5f, 0.5f };

private:
	friend class GameObject;
	friend class MonoBehavior<RectTransform, Transform>;
	RectTransform() = default;
	RectTransform(RectTransform&) = default;

public:
	~RectTransform() {}

	void Start()
	{
	}

	void Update()
	{
	}
};