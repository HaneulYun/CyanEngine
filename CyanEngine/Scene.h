#pragma once

class Environment
{
public:
	Light* sunSources{ nullptr };
};

class Scene
{
public:
	bool isDirty{ true };

public:
	std::deque<GameObject*> gameObjects;
	ObjectRenderManager objectRenderManager;
	SpatialPartitioningManager spatialPartitioningManager;

	std::queue<GameObject*> creationQueue;
	std::priority_queue<GameObject*> deletionQueue;
	std::vector<GameObject*> textObjects;

public:
	FrameResourceManager frameResourceManager;
	LightResourceManager lightResourceManager;

	Environment environment;
	

public:
	static Scene* scene;
	Camera* camera{ nullptr };

public:
	Scene();
	~Scene();

	virtual void Start();
	virtual void Update();

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	// GameObject Management
	GameObject* AddGameObject(GameObject*);

	GameObject* CreateEmpty();
	GameObject* Duplicate(GameObject*);
	static GameObject* CreateEmptyPrefab();
	static GameObject* DuplicatePrefab(GameObject*);

	GameObject* CreateUI();
	GameObject* CreateImage();
	static GameObject* CreateImagePrefab();

	void PushDelete(GameObject*);
	void Delete(GameObject*);
};
