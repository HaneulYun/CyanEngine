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
		xPos = 399;
		yPos = 399;
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
			gameObject->transform->position = { xPos * 0.055f, -yPos * 0.055f, -0.0001f };

	}

	void setName()
	{
		Text* text = gameObject->children[0]->GetComponent<Text>();
		if (text)
		{
			std::wstring wstr(name, &name[MAX_ID_LEN]);
			text->text = wstr;
		}
	}

	//void addChat(char chat[])
	//{
	//	using namespace std::chrono;
	//	Text* text = gameObject->children[1]->GetComponent<Text>();
	//	if (text)
	//	{
	//		std::wstring wstr(chat, &chat[strlen(chat)]);
	//		text->text = wstr;
	//		m_time_out = std::chrono::high_resolution_clock::now() + 1s;
	//	}
	//}
	
	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};