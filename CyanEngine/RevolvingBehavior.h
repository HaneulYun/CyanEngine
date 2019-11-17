#pragma once
#include "framework.h"

class RevolvingBehavior : public Component
{
private:
	XMFLOAT3 axis{ 0.0f, 0.0f, 1.0f };

public:
	GameObject* target{ nullptr };
	float radius{ 10.f };
	float speedRotating{ 30.f };
	float angle{ 0.0f };

private:
	friend class GameObject;
	RevolvingBehavior() {}
	RevolvingBehavior(RevolvingBehavior&) = default;

public:
	~RevolvingBehavior() {}
	virtual Component* Duplicate() { return new RevolvingBehavior; }
	virtual Component* Duplicate(Component* component) { return new RevolvingBehavior(*(RevolvingBehavior*)component); }


	void Start()
	{
	}

	void Update()
	{
		if (SceneManager::scenemanager->GetComponent<SceneManager>()->gameState == START) {
			angle += speedRotating * Time::deltaTime;
			gameObject->transform->position.xmf3 = NS_Vector3::Add(target->transform->position.xmf3,
				XMFLOAT3(radius * cos(angle * PI / 180.0f), radius * sin(angle * PI / 180.0f), 0.0f));
		}
	}
};