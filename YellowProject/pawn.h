#pragma once
#include "framework.h"

class Pawn : public MonoBehavior<Pawn>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	int x{ 0 };
	int y{ 0 };

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
	}

	void SetPositionByIndex(int x, int y)
	{
		this->x = x;
		this->y = y;
		gameObject->transform->position = { 75 * (x - 3.5f), 75 * (y - 3.5f), -1 };

		gameObject->GetComponent<Renderer>()->item->NumFramesDirty = NUM_FRAME_RESOURCES;
		gameObject->GetComponent<Renderer>()->item->World = gameObject->transform->localToWorldMatrix;
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};