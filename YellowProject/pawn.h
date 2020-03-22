#pragma once
#include "framework.h"

class Pawn : public MonoBehavior<Pawn>
{
private /*�� ������ private ������ �����ϼ���.*/:
	int x{ 0 };
	int y{ 0 };

public  /*�� ������ public ������ �����ϼ���.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<Pawn>;
	Pawn() = default;
	Pawn(Pawn&) = default;

public:
	~Pawn() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
		SetPositionByIndex(x, y);
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		if (Input::GetMouseButtonDown(0))
		{
			Vector3 pos = Camera::main->ScreenToWorldPoint(Input::mousePosition);
			int xIndexOnBoard = GetIndexFromPosition(pos.x);
			int yIndexOnBoard = GetIndexFromPosition(pos.y);

			if (x == 0 && xIndexOnBoard == -1 ||
				x == 7 && xIndexOnBoard == 8 ||
				y == 0 && xIndexOnBoard == -1 ||
				y == 7 && xIndexOnBoard == 8)
				return;
			if(abs(x - xIndexOnBoard) + abs(y - yIndexOnBoard) < 2)
				SetPositionByIndex(xIndexOnBoard, yIndexOnBoard);
		}
	}

	void SetPositionByIndex(int x, int y)
	{
		this->x = x;
		this->y = y;
		gameObject->transform->position = { 75 * (x - 3.5f), 75 * (y - 3.5f), -1 };

		gameObject->GetComponent<Renderer>()->item->NumFramesDirty = NUM_FRAME_RESOURCES;
		gameObject->GetComponent<Renderer>()->item->World = gameObject->transform->localToWorldMatrix;
	}

	int GetIndexFromPosition(float d)
	{
		return (d+300) / 75;
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};