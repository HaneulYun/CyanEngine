#pragma once
#include <chrono>
#include "framework.h"
#include "..\CyanServer\protocol.h"

class Pawn : public MonoBehavior<Pawn>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	int max_hp{ 100 };
	int hp{ 100 };
	int level{ 1 };
	int exp{ 0 };

	int x{ 0 };
	int y{ 0 };
	char name[MAX_ID_LEN];

	std::chrono::high_resolution_clock::time_point m_time_out;
	std::chrono::high_resolution_clock::time_point old_heal_time;

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
		old_heal_time = m_time_out = std::chrono::high_resolution_clock::now();
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		if (currentTime > m_time_out)
			gameObject->children[1]->GetComponent<Text>()->text = L"";
		if (currentTime > old_heal_time)
		{
			using namespace std::chrono;
			old_heal_time = std::chrono::high_resolution_clock::now() + 5s;

			HealHealth(max_hp * 0.1);
		}
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
	void HealHealth(int heal)
	{
		if (hp + heal > max_hp)
			hp = max_hp;
		else
			hp += heal;
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

	void addChat(wchar_t chat[])
	{
		using namespace std::chrono;
		Text* text = gameObject->children[1]->GetComponent<Text>();
		if (text)
		{
			std::wstring wstr(chat, &chat[wcslen(chat)]);
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

	void UpdateStatus(std::wstring& text)
	{
		wchar_t wstr[256];
		wsprintf(wstr, L" Level : %d    Exp : %d / %d\n HP : %d / %d", level, exp, 100 * int(pow(2, level-1)), hp, max_hp);
		text = wstr;
	}
};