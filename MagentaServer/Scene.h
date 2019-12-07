#pragma once

class Scene : public Object
{
protected:
	std::deque<GameObject*> gameObjects;
	std::priority_queue<GameObject*> deletionQueue;
public:
	static Scene* scene;

public:
	Scene();
	virtual ~Scene();

	virtual void Start();
	virtual void Update();

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	GameObject* CreateEmpty();
	GameObject* Duplicate(GameObject*);
	GameObject* CreateEmptyPrefab();
	GameObject* DuplicatePrefab(GameObject*);

	void Delete(GameObject*);
};