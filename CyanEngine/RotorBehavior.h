#pragma once
#include "framework.h"

class RotorBehavior : public Component
{
private /*이 영역에 private 변수를 선언하세요.*/:
	GameObject* mainRotorFrame;
	GameObject* tailRotorFrame;

public  /*이 영역에 public 변수를 선언하세요.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<RotorBehavior>;
	RotorBehavior() = default;
	RotorBehavior(RotorBehavior&) = default;

public:
	~RotorBehavior() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{

	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{

	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};