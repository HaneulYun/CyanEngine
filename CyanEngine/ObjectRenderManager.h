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
private:

	std::vector<std::unique_ptr<ObjectsResource>> objectsResources;

public:
	int isDirty{ NUM_FRAME_RESOURCES };
	std::vector<GameObject*> gameObjects;

private:
	std::unique_ptr<ObjectsResource> MakeResource();

public:
	void Update();

	void AddGameObject(GameObject* gameObject);
	ObjectsResource* GetResources();

};

class ObjectRenderManager
{
public:
	std::map<Mesh*, RenderSets> renderObjectsLayer[(int)RenderLayer::Count];

public:
	void Update();

	void AddGameObject(GameObject* gameObject, int layer = 0);
};
