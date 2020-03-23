#pragma once
#include "framework.h"

class Board : public MonoBehavior<Board>
{
private:

public:

private:
	friend class GameObject;
	friend class MonoBehavior<Board>;
	Board() = default;
	Board(Board&) = default;

public:
	~Board() {}

	void Start()
	{
	}

	void Update()
	{
	}
};
