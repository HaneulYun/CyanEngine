#pragma once
#include "framework.h"

class RotorBehavior : public Component
{
private /*�� ������ private ������ �����ϼ���.*/:
	GameObject* mainRotorFrame;
	GameObject* tailRotorFrame;

public  /*�� ������ public ������ �����ϼ���.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<RotorBehavior>;
	RotorBehavior() = default;
	RotorBehavior(RotorBehavior&) = default;

public:
	~RotorBehavior() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{

	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{

	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

