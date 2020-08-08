#pragma once

enum class RenderLayer : int
{
	Opaque = 0,
	SkinnedOpaque,
	Debug,
	Grass,
	BuildPreview,
	UI,
	OnUI,
	Sky,
	Particle,
	Light,
	Count
};

struct BoneWeightData
{
	unsigned int boneIndex;
	double weight;
};
struct FbxModelData
{
	static Vector3 scale;

	std::string name{};

	Scene* scene;

	bool loadedMesh{ false };

	std::vector<SubmeshGeometry> submeshes;

	std::map<int, FbxNode*> nodes;
	std::vector<int> parentIndexer;
	std::map<std::string, int> skeletonIndexer;

	GameObject* rootBoneObject{ nullptr };

	// Assets Management
	void SetName(std::string name) { this->name = name; }
	void SetScene(Scene* scene) { this->scene = scene; }
	void LoadFbx(const char* path);
	void LoadFbxHierarchy(FbxNode* node);
	void LoadFbxHierarchyRecursive(FbxNode* node, GameObject* boneObject, int parentIndex = -1);
	void LoadFbxMesh(FbxNode* node);
};