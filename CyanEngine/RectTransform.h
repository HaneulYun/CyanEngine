#pragma once

#include "CyanFW.h"

class RectTransform : public MonoBehavior<RectTransform, Transform>
{
public:
	float posX{ 0.0f };
	float posY{ 0.0f };
	float posZ{ 0.0f };
	float width{ 100.0f };
	float height{ 100.0f };

	float left{ 0.0f };
	float top{ 0.0f };
	float right{ 0.0f };
	float bottom{ 0.0f };

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
		float parentWidth = CyanFW::Instance()->GetWidth();
		float parentHeight = CyanFW::Instance()->GetHeight();
		if (gameObject->parent)
		{
			auto parentRectTransform = gameObject->parent->GetComponent<RectTransform>();
			if (parentRectTransform)
			{
				parentWidth = parentRectTransform->width;
				parentHeight = parentRectTransform->height;
			}
		}

		float offsetX{ 0 };
		if (!IsEqual(anchorMin.x, anchorMax.x))
		{
			posX = left;
			width = parentWidth * (anchorMax.x - anchorMin.x) - right - left;
		}
		else
		{
			offsetX = width * pivot.x;
		}
		localToWorldMatrix._11 = width / parentWidth;
		localToWorldMatrix._41 = anchorMin.x + (posX - offsetX) / parentWidth;

		float offsetY{ 0 };
		if (!IsEqual(anchorMin.y, anchorMax.y))
		{
			posY = bottom;
			height = parentHeight * (anchorMax.y - anchorMin.y) - top - bottom;
		}
		else
		{
			offsetY = height * pivot.y;
		}
		localToWorldMatrix._22 = height / parentHeight;
		localToWorldMatrix._42 = anchorMin.y + (posY - offsetY) / parentHeight;

		localToWorldMatrix._43 = posZ;
	}
	static Matrix4x4 Transform(Matrix4x4 mtx)
	{
		mtx._11 = mtx._11 * 2;
		mtx._22 = mtx._22 * 2;
		mtx._41 = mtx._41 * 2 - 1;
		mtx._42 = mtx._42 * 2 - 1;
		return mtx;
	}
};