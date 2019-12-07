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

	void Start(/*초기화 코드를 작성하세요.*/)
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

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

