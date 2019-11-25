#pragma once

class MovingBehavior : public MonoBehavior<MovingBehavior>
{
private:
	// �� ������ private ������ �����ϼ���.

public:
	// �� ������ public ������ �����ϼ���.
	Vector3 target{ 0, 0, 0 };
	Vector3 old;

	float speed{ 15 };
	bool collision{ false };
private:
	friend class GameObject;
	friend class MonoBehavior<MovingBehavior>;
	MovingBehavior() = default;
	MovingBehavior(MovingBehavior&) = default;

public:
	~MovingBehavior() {}

	void Start()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
		old = (target - gameObject->transform->position).Normalize();
	}

	void Update()
	{
		if (SceneManager::scenemanager->GetComponent<SceneManager>()->gameState == START) {
			// ������Ʈ �ڵ带 �ۼ��ϼ���.

			Vector3 v = (target - gameObject->transform->position).Normalize();

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
