#pragma once

struct ObjectsResource
{
	bool isDirty{ true };

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
	bool isDirty{ true };

	std::vector<GameObject*> gameObjects;
	std::vector<std::unique_ptr<ObjectsResource>> objectsResources;

public:
	void Update();

	void AddGameObject(GameObject* gameObject);
};

class ObjectRenderManager
{
	bool isDirty{ true };

	std::map<Mesh*, RenderSets> renderObjectsLayer[(int)RenderLayer::Count];

public:
	void Update();

	void AddGameObject(GameObject* gameObject, int layer = 0);
};
