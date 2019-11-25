#pragma once

class MovingBehavior : public MonoBehavior<MovingBehavior>
{
private:
	// 이 영역에 private 변수를 선언하세요.

public:
	// 이 영역에 public 변수를 선언하세요.
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
		// 초기화 코드를 작성하세요.
		old = (target - gameObject->transform->position).Normalize();
	}

	void Update()
	{
		if (SceneManager::scenemanager->GetComponent<SceneManager>()->gameState == START) {
			// 업데이트 코드를 작성하세요.

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
	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};
