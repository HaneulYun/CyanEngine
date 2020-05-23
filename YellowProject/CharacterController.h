#pragma once
#include <iostream>
#include "..\CyanEngine\framework.h"

class CharacterController : public MonoBehavior<CharacterController>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	int xPos, yPos;
public  /*이 영역에 public 변수를 선언하세요.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<CharacterController>;
	CharacterController() = default;
	CharacterController(CharacterController&) = default;

public:
	~CharacterController() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
		xPos = 0;
		yPos = 0;
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		if (Input::GetKeyDown(KeyCode::W) && yPos > 0)
		{
			yPos--;
		}

		if (Input::GetKeyDown(KeyCode::S) && yPos < 399)
		{
			yPos++;
		}

		if (Input::GetKeyDown(KeyCode::D) && xPos < 399)
		{
			xPos++;
		}

		if (Input::GetKeyDown(KeyCode::A) && xPos > 0)
		{
			xPos--;
		}
		gameObject->transform->position = { xPos * 0.075f, -yPos * 0.075f, -0.1f };
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};