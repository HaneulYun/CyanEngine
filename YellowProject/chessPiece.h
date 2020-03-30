#pragma once
#include "framework.h"

class chessPiece : public MonoBehavior<chessPiece>
{
private:
	int x{ 0 }, y{ 0 };
public:

private:
	friend class GameObject;
	friend class MonoBehavior<chessPiece>;
	chessPiece() = default;
	chessPiece(chessPiece&) = default;

public:
	~chessPiece() {}

	void Start()
	{
		move(0, 0);
	}

	void Update()
	{
	}

	void move(int inx, int iny)
	{
		x = inx;
		y = iny;

		float zeroPosition = 37.5 * 7.0f;

		gameObject->transform->position = { -zeroPosition + (75.f * x), zeroPosition + (75.f * -y), -1.0f };

		gameObject->GetComponent<Renderer>()->item->NumFramesDirty = NUM_FRAME_RESOURCES;
		gameObject->GetComponent<Renderer>()->item->World = gameObject->transform->localToWorldMatrix;
	}
};
