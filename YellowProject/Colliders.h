#pragma once
#include "..\CyanEngine\framework.h"

class Colliders : public MonoBehavior<Colliders>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:

protected:
	friend class GameObject;
	friend class MonoBehavior<Colliders>;
	Colliders() = default;
	Colliders(Colliders&) = default;

public:
	~Colliders() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};