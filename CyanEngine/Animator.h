#pragma once

class Animator : public MonoBehavior<Animator>
{
public:
	AnimatorController* controller;

public:
	std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
	AnimationState* state;
	float TimePos = 0.0f;

	void UpdateSkinnedAnimation(float dt)
	{
		TimePos += dt;

		if (TimePos > controller->GetClipEndTime(state))
			TimePos -= controller->GetClipEndTime(state);

		controller->GetFinalTransforms(state, TimePos, FinalTransforms);
	}

private:
	friend class GameObject;
	friend class MonoBehavior<Animator>;
	Animator() = default;
	Animator(Animator&) = default;

public:
	~Animator() {}

	void Start()
	{
		controller->mBoneOffsets = gameObject->GetComponent<SkinnedMeshRenderer>()->mesh->BoneOffsets;
		controller->mBoneHierarchy = gameObject->GetComponent<SkinnedMeshRenderer>()->mesh->ParentIndexer;
		FinalTransforms.resize(controller->mBoneOffsets.size());
	}
	void Update() {}
};