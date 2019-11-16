#pragma once
#include "framework.h"
#include "Bullet.h"

class StarGuadian : public Component
{
private:
	// �� ������ private ������ �����ϼ���.

public:
	// �� ������ public ������ �����ϼ���.
	GameObject* bullet{ nullptr };

private:
	friend class GameObject;
	StarGuadian() {}
	StarGuadian(StarGuadian&) = default;

public:
	~StarGuadian() {}
	virtual Component* Duplicate() { return new StarGuadian; };
	virtual Component* Duplicate(Component* component) { return new StarGuadian(*(StarGuadian*)component); }

	void Starguadiant()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
	}

	void Update()
	{
		// ������Ʈ �ڵ带 �ۼ��ϼ���.
		if (Input::GetMouseButtonDown(0)) {
			GameObject* object = Instantiate(bullet);
			Vector3 mousepostion = Camera::main->ScreenToWorldPoint(Input::mousePosition);

			object->transform->position = gameObject->transform->position;
			object->GetComponent<Bullet>()->direction.xmf3 = NS_Vector3::Subtract(mousepostion.xmf3, object->transform->position.xmf3);
		}

		XMFLOAT3 position = (gameObject->transform->position / 2).xmf3;
		XMFLOAT3 lookAt = position;
		position.z = -10;
		XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

		Camera::main->GenerateViewMatrix(position, lookAt, up);
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

