#pragma once

enum class RenderLayer : int
{
	Opaque = 0,
	SkinnedOpaque,
	Debug,
	Sky,
	Count
};

struct BoneWeightData
{
	unsigned int boneIndex;
	double weight;
};
struct FbxModelData
{
	std::string name{};

	bool loadedMesh{ false };

	std::vector<SubmeshGeometry> submeshes;

	std::map<int, FbxNode*> nodes;
	std::vector<int> parentIndexer;
	std::map<std::string, int> skeletonIndexer;

	// Assets Management
	void SetName(std::string name) { this->name = name; }
	void LoadFbx(const char* path);
	void LoadFbxHierarchy(FbxNode* node);
	void LoadFbxHierarchyRecursive(FbxNode* node, int parentIndex = -1);
	void LoadFbxMesh(FbxNode* node);
};