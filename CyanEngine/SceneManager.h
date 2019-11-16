#pragma once
#include "framework.h"

class SceneManager : public Component
{
private:

public:
	GameObject* gameObject{ nullptr };

	static Material* materials[16];

private:
	friend class GameObject;
	SceneManager() = default;
	SceneManager(SceneManager&) = default;

public:
	~SceneManager() {}

	void Start()
	{
		int xObjects = 1, yObjects = 1, zObjects = 1, i = 0;
		//for (int x = -xObjects; x <= xObjects; x++)
		//	for (int y = -yObjects; y <= yObjects; y++)
		//		for (int z = -zObjects; z <= zObjects; z++)
		//		{
		//			GameObject* instance = Instantiate(gameObject);
		//
		//			instance->transform->position = Vector3{ 10.0f * x, 10.0f * y, 10.0f * z };
		//			//instance->GetComponent<Transform>()->position = XMFLOAT3{ 10.0f * x, 10.0f * y, 10.0f * z };
		//			
		//			instance->GetComponent<RotatingBehavior>()->speedRotating = 0;// 10.0f * (i++ % 10);
		//		}
	}

	void Update()
	{
		static bool instancing = true;
		static float time = 0;
		static const float boundary = 0.0;
		time += Time::deltaTime;

		if (Input::GetMouseButtonDown(0))
			instancing = !instancing;

		if (instancing)
		{
			if (time > boundary)
			{
				auto instance = Instantiate(gameObject);
				float x = Random::Range(-500, 500);
				float y = Random::Range(-500, 500);
				float z = Random::Range(10, 500);
				instance->transform->position = Vector3{ x, y, z };

				instance->GetComponent<Renderer>()->material = materials[Random::Range(0, 15)];
				instance->GetComponent<RotatingBehavior>()->speedRotating = 90;

				time -= boundary;
			}
		}
	}

	virtual Component* Duplicate() { return new SceneManager; };
	virtual Component* Duplicate(Component* component) { return new SceneManager(*(SceneManager*)component); }
};

