#pragma once
#include <iostream>
#include <chrono>
#include "..\CyanEngine\framework.h"
#include "Network.h"

class CharacterController : public MonoBehavior<CharacterController>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	int xPos, yPos;
	int level, hp, exp;
	bool player{ false };
	char name[MAX_ID_LEN];

	GameObject* attackEffect[4] = { nullptr };
	chrono::high_resolution_clock::time_point effectEndTime;
	chrono::high_resolution_clock::time_point attackCooltime;

	Text* coord{ nullptr };
	Text* levelandhp{ nullptr };
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
		level = 0;
		hp = 0;
		exp = 0;

		effectEndTime = chrono::high_resolution_clock::now();
	}

	void attack()
	{
		for (int i = 0; i < 4; ++i)
		{
			if(attackEffect[i])
			attackEffect[i]->SetActive(true);
		}
		effectEndTime = chrono::high_resolution_clock::now() + 300ms;
		attackCooltime = chrono::high_resolution_clock::now() + 1s;
	}

	void attackCancel()
	{
		for (int i = 0; i < 4; ++i)
		{
			if(attackEffect[i])
				attackEffect[i]->SetActive(false);
		}
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		if (player)
		{
			if (levelandhp != nullptr)
			{
				char uitext[50];
				sprintf(uitext, "Level: %d, EXP: %d, HP: %d", level, exp, hp);
				string uis = uitext;
				levelandhp->text.assign(uis.begin(), uis.end());
			}
			if (coord != nullptr)
			{
				char coordtext[20];
				sprintf(coordtext, "(%d, %d)", xPos, yPos);
				string coords = coordtext;
				coord->text.assign(coords.begin(), coords.end());
			}
			if (Input::GetKeyDown(KeyCode::W) && yPos > 0)
			{
				if (Network::network->isConnect && Network::network->pressChatButton == false)
					Network::network->send_move_packet(D_UP);
				else
					yPos--;
				attackCancel();
			}

			else if (Input::GetKeyDown(KeyCode::S) && yPos < WORLD_HEIGHT - 1)
			{
				if (Network::network->isConnect && Network::network->pressChatButton == false)
					Network::network->send_move_packet(D_DOWN);
				else
					yPos++;
				attackCancel();
			}

			else if (Input::GetKeyDown(KeyCode::D) && xPos < WORLD_WIDTH - 1)
			{
				if (Network::network->isConnect && Network::network->pressChatButton == false)
					Network::network->send_move_packet(D_RIGHT);
				else
					xPos++;
				attackCancel();
			}

			else if (Input::GetKeyDown(KeyCode::A) && xPos > 0)
			{
				if (Network::network->isConnect && Network::network->pressChatButton == false)
					Network::network->send_move_packet(D_LEFT);
				else
					xPos--;
				attackCancel();
			}
			if (Input::GetKeyDown(KeyCode::Q) && chrono::high_resolution_clock::now() >= attackCooltime)
			{
				if (Network::network->isConnect && Network::network->pressChatButton == false)
					Network::network->send_attack_packet();
				attack();
			}

		}
		if (!Network::network->isConnect)
			gameObject->transform->position = { xPos * 1.0f, -yPos * 1.0f, -0.0001f };

		if (chrono::high_resolution_clock::now() >= effectEndTime)
		{
			attackCancel();
		}
	}

	void setName()
	{
		Text* text = gameObject->children[0]->GetComponent<Text>();
		if (text)
		{
			std::string namestr = name;
			text->text.assign(namestr.begin(), namestr.end());
		}
	}

	void renewTextPos(int x, int y)
	{
		gameObject->children[0]->GetComponent<RectTransform>()->anchorMin = { (x - 1.0f) / 20.f, (21.5f - y) / 20.f };
		gameObject->children[0]->GetComponent<RectTransform>()->anchorMax = { (x + 2.0f) / 20.f, (20.5f - y) / 20.f };
	}
	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};