#pragma once

class Scene
{
protected:
	std::deque<GameObject*> gameObjects;
	RendererManager* rendererManager{ nullptr };

	std::priority_queue<GameObject*> deletionQueue;

public:
	static Scene* scene;

public:
	Scene();
	virtual ~Scene();

	virtual void Start();
	virtual void Update();
	virtual void Render();

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	GameObject* AddGameObject(GameObject*);
	GameObject* CreateEmpty(bool = true);
	GameObject* Duplicate(GameObject*);
	GameObject* CreateEmptyPrefab();
	GameObject* DuplicatePrefab(GameObject*);

	void PushDelete(GameObject*);
	void Delete(GameObject*);
};