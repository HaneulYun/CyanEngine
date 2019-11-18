#pragma once

class Scene
{
protected:
	std::deque<GameObject*> gameObjects;
	RendererManager* rendererManager;

public:
	Scene();
	virtual ~Scene();

	virtual void Start();
	virtual void Update();
	virtual void Render();
	virtual void Destroy();

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	GameObject* CreateGameObject();
	GameObject* AddGameObject();
	GameObject* AddGameObject(GameObject* gameObject);
	void RemoveGameObject(GameObject* gameObject);
};