#pragma once
#include "framework.h"

class Pawn : public MonoBehavior<Pawn>
{
private:

public:
	Vector3 pos;
	Network* network;

private:
	friend class GameObject;
	friend class MonoBehavior<Pawn>;
	Pawn() = default;
	Pawn(Pawn&) = default;

public:
	~Pawn() { }

	void Start()
	{
		pos = { -2100 / 8, -2100 / 8, -1 };
		gameObject->transform->position = pos;
		gameObject->GetComponent<Renderer>()->item->NumFramesDirty = NUM_FRAME_RESOURCES;
		gameObject->GetComponent<Renderer>()->item->World = gameObject->transform->localToWorldMatrix;
	}

	void Update()
	{
		if (Input::GetKeyDown(KeyCode::W))
		{
			pos = network->communicate(MOVE_UP, pos);
		}
		else if (Input::GetKeyDown(KeyCode::A))
		{
			pos = network->communicate(MOVE_LEFT, pos);
		}
		else if (Input::GetKeyDown(KeyCode::S))
		{
			pos = network->communicate(MOVE_DOWN, pos);
		}
		else if (Input::GetKeyDown(KeyCode::D))
		{
			pos = network->communicate(MOVE_RIGHT, pos);
		}
		gameObject->transform->position = pos;
		gameObject->GetComponent<Renderer>()->item->NumFramesDirty = NUM_FRAME_RESOURCES;
		gameObject->GetComponent<Renderer>()->item->World = gameObject->transform->localToWorldMatrix;
	}
};
