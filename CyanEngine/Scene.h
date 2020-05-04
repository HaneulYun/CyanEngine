#pragma once

class Scene
{
public:
	bool isDirty{ true };

public:
	std::deque<GameObject*> gameObjects;
	ObjectRenderManager objectRenderManager;

	std::queue<GameObject*> creationQueue;
	std::priority_queue<GameObject*> deletionQueue;
	std::vector<GameObject*> textObjects;

public:
	FrameResourceManager frameResourceManager;

public:
	static Scene* scene;
	Camera* camera{ nullptr };

public:
	Scene();
	virtual ~Scene();

	virtual void Start();
	virtual void Update();

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	// GameObject Management
	GameObject* AddGameObject(GameObject*);

	GameObject* CreateEmpty();
	GameObject* Duplicate(GameObject*);
	GameObject* CreateEmptyPrefab();
	GameObject* DuplicatePrefab(GameObject*);

	GameObject* CreateUI();
	GameObject* CreateImage();
	GameObject* CreateImagePrefab();

	void PushDelete(GameObject*);
	void Delete(GameObject*);
};
