#pragma once
#include "..\CyanEngine\framework.h"

class Building : public MonoBehavior<Building>
{
private /*이 영역에 private 변수를 선언하세요.*/:
public  /*이 영역에 public 변수를 선언하세요.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<Building>;
	Building() = default;
	Building(Building&) = default;

public:
	~Building() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
	}

	void OnTriggerEnter(GameObject* collision)
	{
		Debug::Log("TriggerEnter\n");
	}

	void OnTriggerStay(GameObject* collision)
	{
		Debug::Log("TriggerStay\n");
	}

	void OnTriggerExit(GameObject* collision)
	{
		Debug::Log("TriggerExint\n");
	}
	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};