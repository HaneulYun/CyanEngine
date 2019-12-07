#pragma once

class WhirlingBehavior : public MonoBehavior<WhirlingBehavior>
{
private:
	XMFLOAT3 axis{ 0.0f, 0.0f, 1.0f };

public:
	GameObject* target{ nullptr };
	float radius{ 200.f };
	float speedRotating{ 30.f };
	float angle{ 0.0f };

private:
	friend class GameObject;
	friend class MonoBehavior<WhirlingBehavior>;
	WhirlingBehavior() = default;
	WhirlingBehavior(WhirlingBehavior&) = default;

public:
	~WhirlingBehavior() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
		speedRotating = Random::Range(30.f, 50.f);
		angle = Random::Range(0.0f, 360.0f);

	}

	void Update()
	{
		if (radius > 0.f)
		{
			angle += speedRotating * Time::deltaTime;
			gameObject->transform->position.xmf3 = NS_Vector3::Add(target->transform->position.xmf3,
				XMFLOAT3(radius * cos(angle * PI / 180.0f), radius * sin(angle * PI / 180.0f), 0.0f));
			radius -= 0.01f;
		}
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

