#pragma once
#include <iostream>
#include "..\CyanEngine\framework.h"

class CharacterController : public MonoBehavior<CharacterController>
{
private /*�� ������ private ������ �����ϼ���.*/:
	int xPos, yPos;
public  /*�� ������ public ������ �����ϼ���.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<CharacterController>;
	CharacterController() = default;
	CharacterController(CharacterController&) = default;

public:
	~CharacterController() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
		xPos = 0;
		yPos = 0;
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
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

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};