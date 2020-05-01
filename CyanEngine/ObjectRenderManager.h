#pragma once

struct ObjectsResource
{
	ObjectsResource() = default;
	ObjectsResource(const ObjectsResource&) = delete;
	ObjectsResource& operator=(const ObjectsResource&) = delete;
	~ObjectsResource() = default;

	std::unique_ptr<UploadBuffer<InstanceData>> InstanceBuffer{ nullptr };
	std::unique_ptr<UploadBuffer<SkinnnedData>> SkinnedBuffer{ nullptr };
	std::unique_ptr<UploadBuffer<MatIndexData>> MatIndexBuffer{ nullptr };
};

struct RenderSets
{
	int isDirty{ NUM_FRAME_RESOURCES };
	int activatedInstance{ 0 };

	std::vector<GameObject*> gameObjects;
	std::vector<std::unique_ptr<ObjectsResource>> objectsResources;

private:
	std::unique_ptr<ObjectsResource> MakeResource();

public:
	void Update();

	void AddGameObject(GameObject* gameObject);
};

class ObjectRenderManager
{
public:
	std::map<Mesh*, RenderSets> renderObjectsLayer[(int)RenderLayer::Count];

public:
	void Update();

	void AddGameObject(GameObject* gameObject, int layer = 0);
};
