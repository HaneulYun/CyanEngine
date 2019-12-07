#pragma once
#include "framework.h"

class Enemy : public MonoBehavior<Enemy>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<Enemy>;
	Enemy() = default;
	Enemy(Enemy&) = default;

public:
	~Enemy() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void OnTriggerEnter(GameObject* collision)
	{
		if (!collision->GetComponent<Bullet>())
			return;
		Destroy(gameObject);
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

