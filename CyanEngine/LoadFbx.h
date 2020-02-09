#pragma once
#include "framework.h"
#include "AnimationController.h"


class LoadFbx : public MonoBehavior<LoadFbx>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
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

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/) { }

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/) { }

	void LoadFbxFileName(const char* fileName);

	void ProcessNode(FbxNode* node, FbxNodeAttribute::EType type);

	void ProcessSkeleton(FbxNode* node);

	void ProcessSkeletonHierarchyRecursively(FbxNode* node, int myIndex, int parentIndex);

	void ProcessMesh(FbxNode* node);

	void ProcessAnimation(FbxNode* node, std::string animationName, float stop, float frameRate, AnimationClip& clip);

	XMFLOAT4X4 ToXMfloat4x4(FbxAMatrix& fbxMatrix);

	XMFLOAT3 ToXMfloat3(FbxVector4& fbxVector4);

	XMFLOAT4 ToXMfloat4(FbxVector4& fbxVector4);
	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};