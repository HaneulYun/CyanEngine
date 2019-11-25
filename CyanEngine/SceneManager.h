#pragma once
#include "framework.h"

class SceneManager : public MonoBehavior<SceneManager>
{
private:

public:
	GameObject* gameObject{ nullptr };

private:
	friend class GameObject;
	friend class MonoBehavior<SceneManager>;
	SceneManager() = default;
	SceneManager(SceneManager&) = default;

public:
	~SceneManager() {}

	void Start()
	{
		gameObject->AddComponent<RotatingBehavior>();

		for (int i = 0; i < 100; ++i)
		{
			auto instance = Instantiate(gameObject);
			float x = Random::Range(-1500, 1500);
			float y = Random::Range(-1500, 1500);
			float z = Random::Range(-1000, 2000);
			instance->transform->position = Vector3{ x, y, z };

			float r = Random::Range(0.0f, 1.0f);
			float g = Random::Range(0.0f, 1.0f);
			float b = Random::Range(0.0f, 1.0f);
			instance->GetComponent<Renderer>()->material->albedo = RANDOM_COLOR;
			instance->GetComponent<RotatingBehavior>()->speedRotating = Random::Range(-90.0f, 90.0f);
			instance->AddComponent<MovingBehavior>();
			instance->AddComponent<BoxCollider>()->extents = { 7, 7, 7 };
		}
	}

	void Update()
	{
		static bool instancing = false;
		static float time = 0;
		static const float boundary = 0.0;
		time += Time::deltaTime;

		if (Input::GetMouseButtonDown(0))
			instancing = !instancing;

		if (instancing)
		{
			if (time > boundary)
			{
				for (int i = 0; i < 1; ++i)
				{
					auto instance = Instantiate(gameObject);
					float x = Random::Range(-1500, 1500);
					float y = Random::Range(-1500, 1500);
					float z = Random::Range(-1000, 2000);
					instance->transform->position = Vector3{ x, y, z };
		
					float r = Random::Range(0.0f, 1.0f);
					float g = Random::Range(0.0f, 1.0f);
					float b = Random::Range(0.0f, 1.0f);
					instance->GetComponent<Renderer>()->material->albedo = RANDOM_COLOR;
					instance->GetComponent<RotatingBehavior>()->speedRotating = Random::Range(-90.0f, 90.0f);
		
					instance->AddComponent<MovingBehavior>();
				}
				time -= boundary;
			}
		}
	}
};

