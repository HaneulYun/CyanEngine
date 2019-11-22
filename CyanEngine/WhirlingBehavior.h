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

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
		target = SceneManager::scenemanager->GetComponent<SceneManager>()->star;

		RevolvingBehavior* revolvingBehavior = gameObject->AddComponent<RevolvingBehavior>();
		revolvingBehavior->target = target;
		revolvingBehavior->radius = 200;
		revolvingBehavior->speedRotating = Random::Range(30.f, 50.f);
		revolvingBehavior->angle = Random::Range(0.0f, 360.0f);

	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		UpdateRadius();
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

