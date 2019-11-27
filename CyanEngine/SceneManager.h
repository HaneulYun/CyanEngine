#pragma once
#include "framework.h"

class SceneManager : public MonoBehavior<SceneManager>
{
private:
	TextureShader* shader;

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
	}

	void Update()
	{
	}
};

