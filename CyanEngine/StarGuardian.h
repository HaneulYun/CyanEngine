#pragma once
#include "framework.h"
#include "Bullet.h"

class StarGuardian : public Component
{
private:
	// �� ������ private ������ �����ϼ���.

public:
	// �� ������ public ������ �����ϼ���.
	GameObject* bullet[5]{ nullptr, };

private:
	friend class GameObject;
	StarGuardian() {}
	StarGuardian(StarGuardian&) = default;

public:
	~StarGuardian() {}
	virtual Component* Duplicate() { return new StarGuardian; };
	virtual Component* Duplicate(Component* component) { return new StarGuardian(*(StarGuardian*)component); }

	void Start()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
	}

	void Update()
	{
		// ������Ʈ �ڵ带 �ۼ��ϼ���.
		
	}

	GameObject* Shoot(int type, Vector3 direction)
	{
		GameObject* object = Instantiate(bullet[type]);
		object->transform->position = gameObject->transform->position;
		if (type == 3)
			object->transform->position += direction.Normalize() * 180.0f;
		object->GetComponent<Bullet>()->direction = direction;

		return object;
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};