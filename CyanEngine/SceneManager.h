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

	void Start();

	void Update()
	{
	}
};

