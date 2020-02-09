#pragma once
#include "framework.h"
#include "AnimationController.h"


class LoadFbx : public MonoBehavior<LoadFbx>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	std::vector<Bone> skeleton;

	std::vector<int> boneHierarchy;
	std::vector<DirectX::XMFLOAT4X4> boneOffsets;
	std::unordered_map<std::string, AnimationClip> animations;

private:
	friend class GameObject;
	friend class MonoBehavior<LoadFbx>;
	LoadFbx() = default;
	LoadFbx(LoadFbx&) = default;

public:
	~LoadFbx() {}

	void Start(/*초기화 코드를 작성하세요.*/) { }

	void Update(/*업데이트 코드를 작성하세요.*/) { }

	void LoadFbxFileName(const char* fileName);

	void ProcessNode(FbxNode* node, FbxNodeAttribute::EType type);

	void ProcessSkeleton(FbxNode* node);

	void ProcessSkeletonHierarchyRecursively(FbxNode* node, int myIndex, int parentIndex);

	void ProcessMesh(FbxNode* node);

	void ProcessAnimation(FbxNode* node, std::string animationName, float stop, float frameRate, AnimationClip& clip);

	XMFLOAT4X4 ToXMfloat4x4(FbxAMatrix& fbxMatrix);

	XMFLOAT3 ToXMfloat3(FbxVector4& fbxVector4);

	XMFLOAT4 ToXMfloat4(FbxVector4& fbxVector4);
	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};