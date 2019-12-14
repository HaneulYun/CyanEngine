#pragma once
#include "framework.h"

class SceneManager : public MonoBehavior<SceneManager>
{
public:
	GameObject* cube{ nullptr };

private:
	friend class GameObject;
	friend class MonoBehavior<SceneManager>;
	SceneManager() = default;
	SceneManager(SceneManager&) = default;

public:
	~SceneManager() {}

	void Start()
	{
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
				for (int i = 0; i < 1; ++i)
				{
					auto instance = Instantiate(cube);
					float x = Random::Range(-15.f, 15.f);
					float y = Random::Range(-15.f, 15.f);
					instance->transform->position = Vector3{ x, y, 0 };

					float r = Random::Range(0.0f, 1.0f);
					float g = Random::Range(0.0f, 1.0f);
					float b = Random::Range(0.0f, 1.0f);
					instance->GetComponent<RotatingBehavior>()->speedRotating = Random::Range(-90.0f, 90.0f);

					instance->AddComponent<MovingBehavior>();
				}
				time -= boundary;
			}
		}
	}
};

