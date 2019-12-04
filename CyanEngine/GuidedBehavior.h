#pragma once
#include "framework.h"

class GuidedBehavior : public MonoBehavior<GuidedBehavior>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	GameObject* target;
	Vector3 old;
	// ��� enemy�� �� �� �ִ� ������ �ʿ���

	float speed{ 15 };
	bool collision{ false };
private:
	friend class GameObject;
	friend class MonoBehavior<GuidedBehavior>;
	GuidedBehavior() = default;
	GuidedBehavior(GuidedBehavior&) = default;

public:
	~GuidedBehavior() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
		// ��� enemy�� �Ÿ��� ���Ͽ� ���� ����� ���� Ÿ������ ��´�!
		old = (target->transform->position - gameObject->transform->position).Normalize();
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		if (SceneManager::scenemanager->GetComponent<SceneManager>()->gameState == START) {
			// ������Ʈ �ڵ带 �ۼ��ϼ���.

			Vector3 v = (target->transform->position - gameObject->transform->position).Normalize();

			auto t = NS_Vector3::Angle(v.xmf3, old.xmf3);
			if (t > 30)
			{
				gameObject->transform->position = Vector3{ 0, 0, 0 };
				speed = 0;

			}

			if (speed)
			{
				gameObject->transform->position = gameObject->transform->position + v * speed * Time::deltaTime;
			}

			old = v;
		}
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

