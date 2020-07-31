#pragma once
#include "..\CyanEngine\framework.h"

class Building : public MonoBehavior<Building>
{
private /*�� ������ private ������ �����ϼ���.*/:
public  /*�� ������ public ������ �����ϼ���.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<Building>;
	Building() = default;
	Building(Building&) = default;

public:
	~Building() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
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
	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};