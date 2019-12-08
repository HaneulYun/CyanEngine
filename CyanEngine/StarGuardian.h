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
		// Rotate StarGuardian
		Vector3 axis{ 0.0f, 1.0f, 0.0f };
		float angle = NS_Vector3::Angle(axis.xmf3, direction.xmf3);

		if (NS_Vector3::CrossProduct(axis.xmf3, direction.xmf3).z < 0.f)
			angle = -angle;

		gameObject->transform->up = Vector3{ 0.0f,1.0f,0.0f };
		gameObject->transform->right = Vector3{ 1.0f,0.0f,0.0f };
		gameObject->transform->forward = Vector3{ 0.0f,0.0f,1.0f };
		gameObject->transform->Rotate(Vector3{ 0.0f,0.0f,1.0f }.xmf3, angle);

		GameObject* object = Instantiate(bullet[type]);
		object->transform->position = gameObject->transform->position;
		if (type == 3)
			object->transform->position += direction.Normalize() * 180.0f;
		object->GetComponent<Bullet>()->direction = direction;

		return object;
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};