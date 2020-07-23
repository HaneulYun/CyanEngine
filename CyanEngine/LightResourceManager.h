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

class LightResourceManager
{
private:
	std::vector<std::unique_ptr<ObjectsResource>> light[Light::Type::Count];

public:
	void Update();
};
