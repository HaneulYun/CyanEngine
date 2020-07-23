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

struct LightData
{
	std::unique_ptr<ShadowMap> shadowMap;
	DirectX::BoundingSphere sceneBounds;
};

class LightResourceManager
{
private:
	std::vector<std::unique_ptr<LightData>> light[Light::Type::Count];

public:
	std::unique_ptr<ShadowMap> shadowMap;
	DirectX::BoundingSphere sceneBounds;

	void Init();
	void Update();
};
