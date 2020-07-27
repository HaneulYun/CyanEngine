#include "pch.h"
#include "Animator.h"

void Animator::Start()
{
	controller->mBoneOffsets = gameObject->GetComponentInChildren<SkinnedMeshRenderer>()->mesh->BoneOffsets;
	controller->mBoneHierarchy = gameObject->GetComponentInChildren<SkinnedMeshRenderer>()->mesh->ParentIndexer;
	//FinalTransforms.resize(controller->mBoneOffsets.size());
}
void Animator::Update()
{
	// skinned data
	auto skinnedMesh = gameObject->GetComponentInChildren<SkinnedMeshRenderer>();
	UpdateSkinnedAnimation(Time::deltaTime, skinnedMesh->bones);
}