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
	bool player{ false };
	char name[MAX_ID_LEN];
	Text* coord{ nullptr };
	chrono::high_resolution_clock::time_point chatEndTime;

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
		chatEndTime = chrono::high_resolution_clock::now();
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		if (player)
		{
			if (coord != nullptr)
			{
				char coordtext[20];
				sprintf(coordtext, "(%d, %d)", xPos, yPos);
				string coords = coordtext;
				coord->text.assign(coords.begin(), coords.end());
			}
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

		if (chatEndTime < chrono::high_resolution_clock::now())
		{
			Text* text = gameObject->children[1]->GetComponent<Text>();
			if (text)
			{
				text->text = L"";
			}
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
		gameObject->children[0]->GetComponent<RectTransform>()->anchorMin = { (x - 1) / 21.f, (21.5f - y) / 21.f };
		gameObject->children[0]->GetComponent<RectTransform>()->anchorMax = { (x + 2) / 21.f, (19.5f - y) / 21.f };
		gameObject->children[1]->GetComponent<RectTransform>()->anchorMin = { (x - 1) / 21.f, (20.f - y) / 21.f };
		gameObject->children[1]->GetComponent<RectTransform>()->anchorMax = { (x + 2) / 21.f, (21.f - y) / 21.f };
	}
	
	void addChat(wchar_t chat[])
	{
		Text* text = gameObject->children[1]->GetComponent<Text>();
		if (text)
		{
			std::wstring wstr(chat);
			text->text = wstr;
			chatEndTime = std::chrono::high_resolution_clock::now() + 1s;
		}
	}
	
	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};