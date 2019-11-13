#pragma once
#include "framework.h"

class SceneManager : public Component
{
private:

public:
	GameObject* gameObject;

private:
	friend class GameObject;
	SceneManager() {}

public:
	~SceneManager() {}

	void Start()
	{
		//int xObjects = 1, yObjects = 1, zObjects = 1, i = 0;
		//for (int x = -xObjects; x <= xObjects; x++)
		//	for (int y = -yObjects; y <= yObjects; y++)
		//		for (int z = -zObjects; z <= zObjects; z++)
		//		{
		//			GameObject* instance = Instantiate(gameObject);
		//
		//			instance->transform->position = XMFLOAT3{ 10.0f * x, 10.0f * y, 10.0f * z };
		//			//instance->GetComponent<Transform>()->position = XMFLOAT3{ 10.0f * x, 10.0f * y, 10.0f * z };
		//			
		//			instance->GetComponent<RotatingBehavior>()->speedRotating = 0;// 10.0f * (i++ % 10);
		//		}
	}

	void Update()
	{
		static float time = 0;
		time += Time::deltaTime;
	}

	virtual Component* Duplicate() { return new SceneManager; };
};

