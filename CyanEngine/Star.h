#pragma once
#include "framework.h"
#include "Bullet.h"

class Star : public Component
{
private:
	// �� ������ private ������ �����ϼ���.
	

public:
	// �� ������ public ������ �����ϼ���.
	int health;
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

