#pragma once
#include "framework.h"

class WhirlingBehavior : public MonoBehavior<WhirlingBehavior>
{
private :
public :
	GameObject* target{ nullptr };

private:
	friend class GameObject;
	friend class MonoBehavior<WhirlingBehavior>;
	WhirlingBehavior() = default;
	WhirlingBehavior(WhirlingBehavior&) = default;

public:
	~WhirlingBehavior() {}

	void UpdateRadius()
	{
		gameObject->GetComponent<RevolvingBehavior>()->radius -= 0.01f;
	}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
		target = SceneManager::scenemanager->GetComponent<SceneManager>()->star;

		RevolvingBehavior* revolvingBehavior = gameObject->AddComponent<RevolvingBehavior>();
		revolvingBehavior->target = target;
		revolvingBehavior->radius = 200;
		revolvingBehavior->speedRotating = Random::Range(30.f, 50.f);
		revolvingBehavior->angle = Random::Range(0.0f, 360.0f);

	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		UpdateRadius();
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

