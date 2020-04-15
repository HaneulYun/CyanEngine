#pragma once

struct RenderObjects
{
	std::vector<GameObject*> gameObjects;
};

class Scene
{
private:
	bool isDirty{ true };

protected:
	std::deque<GameObject*> gameObjects;
	std::map<Mesh*, RenderObjects> renderObjects[(int)RenderLayer::Count];

	std::priority_queue<GameObject*> deletionQueue;

public:
	static Scene* scene;

	std::vector<std::unique_ptr<FrameResource>> frameResources;

	std::unordered_map<std::string, std::unique_ptr<Mesh>> geometries;
	std::unordered_map<std::string, std::unique_ptr<Material>> materials;
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
	std::unordered_map<std::string, std::unique_ptr<AnimationClip>> animationClips;
	//std::vector<TextureData> textureData;

	std::vector<GameObject*> allRItems;
	std::vector<GameObject*> renderItemLayer[(int)RenderLayer::Count];

public:
	Scene();
	virtual ~Scene();

	virtual void Start();
	virtual void Update();

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	// GameObject Management
	GameObject* AddGameObject(GameObject*);
	GameObject* CreateEmpty(bool = true);
	GameObject* Duplicate(GameObject*);
	GameObject* CreateEmptyPrefab();
	GameObject* DuplicatePrefab(GameObject*);

	void PushDelete(GameObject*);
	void Delete(GameObject*);
};