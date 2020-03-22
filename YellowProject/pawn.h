#pragma once
#include "framework.h"

class Pawn : public MonoBehavior<Pawn>
{
private:

public:
	int x{ 0 };
	int y{ 0 };

private:
	friend class GameObject;
	friend class MonoBehavior<Pawn>;
	Pawn() = default;
	Pawn(Pawn&) = default;

public:
	~Pawn() {}

	void Start()
	{
		gameObject->transform->position = getPosToBoardPos(x, y);
		gameObject->GetComponent<Renderer>()->item->NumFramesDirty = NUM_FRAME_RESOURCES;
		gameObject->GetComponent<Renderer>()->item->World = gameObject->transform->localToWorldMatrix;
	}

	void Update()
	{
		if (Input::GetMouseButtonDown(0))
		{
			Vector3 cursorPos = Camera::main->ScreenToWorldPoint(Input::mousePosition);
			if (cursorPos.x > -300 && cursorPos.x < 300)
			{
				x = (cursorPos.x + 300) / (600 / 8);
				y = (cursorPos.y + 300) / (600 / 8);
			}
		}
		else if (Input::GetKeyDown(KeyCode::W) && y < 7)
		{
			y++;
		}
		else if (Input::GetKeyDown(KeyCode::A) && x > 0)
		{
			x--;
		}
		else if (Input::GetKeyDown(KeyCode::S) && y > 0)
		{
			y--;
		}
		else if (Input::GetKeyDown(KeyCode::D) && x < 7)
		{
			x++;
		}
		gameObject->transform->position = getPosToBoardPos(x, y);
		gameObject->GetComponent<Renderer>()->item->NumFramesDirty = NUM_FRAME_RESOURCES;
		gameObject->GetComponent<Renderer>()->item->World = gameObject->transform->localToWorldMatrix;
	}

	Vector3 getPosToBoardPos(int x, int y)
	{
		Vector3 boardPos;
		boardPos.x = x * 600 / 8 - (2100/8);
		boardPos.y = y * 600 / 8 - (2100/8);
		boardPos.z = -1;

		return boardPos;
	}
};
