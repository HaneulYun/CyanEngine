#pragma once

#include "CyanFW.h"

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
		localToWorldMatrix._11 = width / CyanFW::Instance()->GetWidth() * 2;
		localToWorldMatrix._22 = height / CyanFW::Instance()->GetHeight() * 2;

		localToWorldMatrix._41 = (anchorMin.x + (posX - width * pivot.x) / CyanFW::Instance()->GetWidth()) * 2 - 1;
		localToWorldMatrix._42 = (anchorMin.y + (posY - height * pivot.y) / CyanFW::Instance()->GetHeight()) * 2 - 1;
		localToWorldMatrix._43 = posZ;
	}
};