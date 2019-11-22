#pragma once
#include "framework.h"

class SceneManager : public Component
{
private:

public:
	GameObject* gameObject{ nullptr };

private:
	friend class GameObject;
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
					auto instance = Instantiate(gameObject);
					float x = Random::Range(-500, 500);
					float y = Random::Range(-500, 500);
					float z = Random::Range(10, 500);
					instance->transform->position = Vector3{ x, y, z };

					float r = Random::Range(0.0f, 1.0f);
					float g = Random::Range(0.0f, 1.0f);
					float b = Random::Range(0.0f, 1.0f);
					instance->GetComponent<Renderer>()->material->albedo = XMFLOAT4(r, g, b, 1.0f);
					instance->GetComponent<RotatingBehavior>()->speedRotating = Random::Range(-90.0f, 90.0f);

					instance->AddComponent<MovingBehavior>();
				}
				time -= boundary;
			}
		}
	}
};

