#pragma once

struct RenderSets
{
private:
	std::vector<std::unique_ptr<ObjectsResource>> objectsResources;

public:
	int isDirty{ NUM_FRAME_RESOURCES };
	int activatedInstance{ 0 };

	std::vector<GameObject*> gameObjects;

private:
	std::unique_ptr<ObjectsResource> MakeResource();

public:
	void Update();
	void Release();

	void AddGameObject(GameObject* gameObject);
	ObjectsResource* GetResources();

};

class ObjectRenderManager
{
public:
	std::map<Mesh*, RenderSets> renderObjectsLayer[(int)RenderLayer::Count];

public:
	void Update();
	void Release();

	void AddGameObject(GameObject* gameObject, int layer = 0);
};
