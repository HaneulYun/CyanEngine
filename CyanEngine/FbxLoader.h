#pragma once

struct SkinnedModelInstance
{
	SkinnedData* SkinnedInfo = nullptr;
	std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
	std::string ClipName;
	float TimePos = 0.0f;

	void UpdateSkinnedAnimation(float dt)
	{
		TimePos += dt;

		float k = SkinnedInfo->GetClipEndTime(ClipName);

		if (TimePos > SkinnedInfo->GetClipEndTime(ClipName))
			TimePos = 0.0f;

		SkinnedInfo->GetFinalTransforms(ClipName, TimePos, FinalTransforms);
	}
};

struct RenderItem
{
	RenderItem() = default;
	RenderItem(const RenderItem& rhs) = delete;

	GameObject* gameObject;

	XMFLOAT4X4 World = MathHelper::Identity4x4();
	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	int NumFramesDirty{ NUM_FRAME_RESOURCES };
	UINT ObjCBIndex{ UINT(-1) };

	Material* Mat{ nullptr };
	MeshGeometry* Geo{ nullptr };

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	UINT IndexCount{ 0 };
	UINT StartIndexLocation{ 0 };
	int BaseVertexLocation{ 0 };

	UINT SkinnedCBIndex = -1;
	SkinnedModelInstance* SkinnedModelInst = nullptr;
};

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

	std::map<int, FbxNode*> nodes;
	std::map<int, std::vector<BoneWeightData>> boneWeightData;
	std::vector<int> parentIndexer;
	std::map<std::string, int> skeletonIndexer;
	std::vector<M3DLoader::Subset> skinnedSubsets;
	std::vector<M3DLoader::M3dMaterial> skinnedMats;
	std::vector<XMFLOAT4X4> boneOffsets;

	// Assets Management
	void LoadFbx(const char* path);
	void LoadFbxHierarchy(FbxNode* node);
	void LoadFbxHierarchyRecursive(FbxNode* node, int parentIndex = -1);
	void LoadFbxMesh(FbxNode* node);
};