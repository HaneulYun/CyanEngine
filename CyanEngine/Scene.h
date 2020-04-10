#pragma once

class Scene
{
private:
	bool isDirty{ true };

protected:
	std::deque<GameObject*> gameObjects;
	std::priority_queue<GameObject*> deletionQueue;


public:
	static Scene* scene;

	std::vector<std::unique_ptr<FrameResource>> frameResources;

	std::vector<std::unique_ptr<RenderItem>> allRItems;
	std::vector<RenderItem*> renderItemLayer[(int)RenderLayer::Count];

public:
	Scene();
	virtual ~Scene();

	virtual void Start();
	virtual void Update();

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