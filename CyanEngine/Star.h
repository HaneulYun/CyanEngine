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
	GameObject* bullet{ nullptr };

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
		if (Input::GetMouseButtonDown(0)) {
			GameObject* object = Instantiate(bullet);
			object->GetComponent<Bullet>()->direction = Vector3(Input::mousePosition.x - FRAME_BUFFER_WIDTH / 2, -(Input::mousePosition.y - FRAME_BUFFER_HEIGHT / 2), Input::mousePosition.z);
		}
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

