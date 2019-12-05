#pragma once
#include "framework.h"

class GuidedBehavior : public MonoBehavior<GuidedBehavior>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	GameObject* target;
	Vector3 old;
	// 모든 enemy를 알 수 있는 변수가 필요함

	float speed{ 15 };
	bool collision{ false };
private:
	friend class GameObject;
	friend class MonoBehavior<GuidedBehavior>;
	GuidedBehavior() = default;
	GuidedBehavior(GuidedBehavior&) = default;

public:
	~GuidedBehavior() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
		// 초기화 코드를 작성하세요.
		// 모든 enemy와 거리를 비교하여 가장 가까운 적을 타겟으로 삼는다!
		old = (target->transform->position - gameObject->transform->position).Normalize();
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		if (SceneManager::scenemanager->GetComponent<SceneManager>()->gameState == START) {
			// 업데이트 코드를 작성하세요.

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

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

