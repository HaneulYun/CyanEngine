#pragma once

class Scene
{
protected:
	std::deque<GameObject*> gameObjects;
	RendererManager* rendererManager;

public:
	static Scene* scene;

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

	//GameObject* CreateEmpty();
	//GameObject* Duplicate(GameObject* gameObject);
	//void Delete(GameObject* gameObject);
};