#pragma once
#include "framework.h"
#include "..\CyanServer\protocol.h"

class Pawn : public MonoBehavior<Pawn>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	int x{ 0 };
	int y{ 0 };
	char name[MAX_ID_LEN];

private:
	friend class GameObject;
	friend class MonoBehavior<Pawn>;
	Pawn() = default;
	Pawn(Pawn&) = default;

public:
	~Pawn() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
		SetPositionByIndex(x, y);
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
	void move(int x, int y)
	{
		this->x = x;
		this->y = y;
		SetPositionCurrentIndex();
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
		gameObject->transform->position = { 75 * (x - 3.5f), 75 * (y - 3.5f), -1 };
	}

};