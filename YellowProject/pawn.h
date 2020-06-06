#pragma once
#include <chrono>
#include "framework.h"
#include "..\CyanServer\protocol.h"

class Pawn : public MonoBehavior<Pawn>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	int x{ 0 };
	int y{ 0 };
	char name[MAX_ID_LEN];

	std::chrono::high_resolution_clock::time_point m_time_out;

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
		m_time_out = std::chrono::high_resolution_clock::now();
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		if (std::chrono::high_resolution_clock::now() > m_time_out)
			gameObject->children[1]->GetComponent<Text>()->text = L"";
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
	void setName()
	{
		Text* text = gameObject->children[0]->GetComponent<Text>();
		if (text)
		{
			std::wstring wstr(name, &name[MAX_ID_LEN]);
			text->text = wstr;
		}
	}

	void addChat(char chat[])
	{
		using namespace std::chrono;
		Text* text = gameObject->children[1]->GetComponent<Text>();
		if (text)
		{
			std::wstring wstr(chat, &chat[strlen(chat)]);
			text->text = wstr;
			m_time_out = std::chrono::high_resolution_clock::now() + 1s;
		}
	}

	void move(int x, int y)
	{
		this->x = x;
		this->y = y;
		//SetPositionCurrentIndex();
	}

	void SetPositionIndex(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	
	void SetPositionCurrentIndex()
	{
		SetPositionByIndex(x, y);
	}

	void SetPositionByIndex(int x, int y)
	{
		this->x = x;
		this->y = y;
		gameObject->transform->position = { (float)x, (float)y, -1 };
	}

};