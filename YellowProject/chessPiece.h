#pragma once
#include "framework.h"

class chessPiece : public MonoBehavior<chessPiece>
{
private:
	float x{ 0 }, y{ 0 };
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
		if (Input::GetKeyDown(KeyCode::W))
		{
			move(0, -1);
		}
		if (Input::GetKeyDown(KeyCode::A))
		{
			move(-1, 0);
		}
		if (Input::GetKeyDown(KeyCode::S))
		{
			move(0, +1);
		}
		if (Input::GetKeyDown(KeyCode::D))
		{
			move(+1, 0);
		}
	}

	void move(int inx, int iny)
	{
		if (0 <= x + inx && x + inx < 8 &&
			0 <= y + iny && y + iny < 8)
		{
			x += inx;
			y += iny;

			float zeroPosition = 37.5 * 7.0f;

			gameObject->transform->position = { -zeroPosition + (75.f * x), zeroPosition + (75.f * -y), -1.0f };

			gameObject->GetComponent<Renderer>()->item->NumFramesDirty = NUM_FRAME_RESOURCES;
			gameObject->GetComponent<Renderer>()->item->World = gameObject->transform->localToWorldMatrix;
		}
	}
};
