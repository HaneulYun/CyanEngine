#pragma once

//struct RenderSets
//{
//	int isDirty{ NUM_FRAME_RESOURCES };
//private:
//	std::vector<std::unique_ptr<ObjectsResource>> objectsResources;
//
//public:
//	int isDirty{ NUM_FRAME_RESOURCES };
//	int activatedInstance{ 0 };
//
//	std::vector<GameObject*> gameObjects;
//
//private:
//	std::unique_ptr<ObjectsResource> MakeResource();
//
//public:
//	void Update();
//
//	void AddGameObject(GameObject* gameObject);
//	ObjectsResource* GetResources();
//
//};
struct LightResource
{
};

class LightData
{
public:
	GameObject* gameObject;
	std::unique_ptr<ShadowMap> shadowMap;
	BoundingSphere sceneBounds;

	std::vector<std::unique_ptr<FrameResource>> frameResources;
	int isDirty{ NUM_FRAME_RESOURCES };
};

class LightResourceManager
{
public:
	std::vector<LightData*> lightObjects[Light::Type::Count];

public:
	void AddGameObject(GameObject* gameObject, int layer = 0);

	void Update();
};
