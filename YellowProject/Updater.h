#pragma once
#include "..\CyanEngine\framework.h"

class Updater : public MonoBehavior<Updater>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	void* dst{ nullptr };
	void* src{ nullptr };
	int size{ 0 };

protected:
	friend class GameObject;
	friend class MonoBehavior<Updater>;
	Updater() = default;
	Updater(Updater&) = default;

public:
	~Updater() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		memcpy(dst, src, size);
	}

	void Set(void* dst, void* src, int size)
	{
		this->dst = dst;
		this->src = src;
		this->size = size;
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};