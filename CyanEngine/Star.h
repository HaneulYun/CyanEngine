#pragma once
#include "framework.h"
#include "Bullet.h"

class Star : public Component
{
private:
	// �� ������ private ������ �����ϼ���.
	int health;

public:
	// �� ������ public ������ �����ϼ���.

private:
	friend class GameObject;
	Star() {}

public:
	~Star() {}
	virtual Component* Duplicate() { return new Star; };

	void Start()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
	}

	void Update()
	{
		// ������Ʈ �ڵ带 �ۼ��ϼ���.
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

