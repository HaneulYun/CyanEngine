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
	std::map<int, std::vector<BoneWeightData>> boneWeightData;
	std::vector<int> parentIndexer;
	std::map<std::string, int> skeletonIndexer;
	std::vector<XMFLOAT4X4> boneOffsets;

	// Assets Management
	void LoadFbx(const char* path);
	void LoadFbxHierarchy(FbxNode* node);
	void LoadFbxHierarchyRecursive(FbxNode* node, int parentIndex = -1);
	void LoadFbxMesh(FbxNode* node);
};