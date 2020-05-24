#pragma once
#include <iostream>
#include "..\CyanEngine\framework.h"
#include "Network.h"

class CharacterController : public MonoBehavior<CharacterController>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	int xPos, yPos;
	bool player{ false };
	char name[MAX_ID_LEN];

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
		if (player)
		{
			if (Input::GetKeyDown(KeyCode::W) && yPos > 0)
			{
				if (Network::network->isConnect)
					Network::network->send_move_packet(D_UP);
				else
					yPos--;
			}

			else if (Input::GetKeyDown(KeyCode::S) && yPos < WORLD_HEIGHT - 1)
			{
				if (Network::network->isConnect)
					Network::network->send_move_packet(D_DOWN);
				else
					yPos++;
			}

			else if (Input::GetKeyDown(KeyCode::D) && xPos < WORLD_WIDTH - 1)
			{
				if (Network::network->isConnect)
					Network::network->send_move_packet(D_RIGHT);
				else
					xPos++;
			}

			else if (Input::GetKeyDown(KeyCode::A) && xPos > 0)
			{
				if (Network::network->isConnect)
					Network::network->send_move_packet(D_LEFT);
				else
					xPos--;
			}

		}
		if (!Network::network->isConnect)
			gameObject->transform->position = { xPos * 0.075f, -yPos * 0.075f, -0.0001f };

	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};