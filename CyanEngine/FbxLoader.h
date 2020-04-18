#pragma once

enum class RenderLayer : int
{
	Opaque = 0,
	SkinnedOpaque,
	Debug,
	Sky,
	Count
};

//struct TextureData
//{
//	std::string name;
//	std::wstring fileName;
//};

struct FbxModelData
{
	struct BoneWeightData
	{
		unsigned int boneIndex;
		double weight;
	};

	bool loadedMesh{ false };

	std::vector<SubmeshGeometry> submeshes;

	std::map<int, FbxNode*> nodes;
	std::vector<int> parentIndexer;
	std::map<std::string, int> skeletonIndexer;

	// Assets Management
	void LoadFbx(const char* path, std::string animName = {});
	void LoadFbxHierarchy(FbxNode* node);
	void LoadFbxHierarchyRecursive(FbxNode* node, int parentIndex = -1);
	void LoadFbxMesh(FbxNode* node);
};