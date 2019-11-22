#pragma once
#include "framework.h"
#include "Bullet.h"

class StarGuardian : public Component
{
private:
	// �� ������ private ������ �����ϼ���.

public:
	// �� ������ public ������ �����ϼ���.
	GameObject* bullet{ nullptr };

private:
	friend class GameObject;
	StarGuardian() {}
	StarGuardian(StarGuardian&) = default;

public:
	~StarGuardian() {}
	virtual Component* Duplicate() { return new StarGuardian; };
	virtual Component* Duplicate(Component* component) { return new StarGuardian(*(StarGuardian*)component); }

	void Starguadiant()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
	}

	void Update()
	{
		// ������Ʈ �ڵ带 �ۼ��ϼ���.
		
	}

	void Shoot(int type)
	{
		//if (Input::GetMouseButtonDown(0)) {
			GameObject* object = Instantiate(bullet);
			Vector3 mousepostion = Camera::main->ScreenToWorldPoint(Input::mousePosition);

			object->transform->position = gameObject->transform->position;
			Vector3 direction = mousepostion - object->transform->position;
			direction.z = 0;
			object->GetComponent<Bullet>()->direction = direction;
		//}
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

