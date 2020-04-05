#pragma once
#include "framework.h"

class Pawn : public MonoBehavior<Pawn>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	int x{ 0 };
	int y{ 0 };

private:
	friend class GameObject;
	friend class MonoBehavior<Pawn>;
	Pawn() = default;
	Pawn(Pawn&) = default;

public:
	~Pawn() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
		SetPositionByIndex(x, y);
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
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

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};