#pragma once

class Animator : public MonoBehavior<Animator>
{
public:
	AnimatorController* controller;

public:
	std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
	AnimationControllerState* state{ nullptr };
	float TimePos = 0.0f;

	std::vector<PastState> pastStates;

	void UpdateSkinnedAnimation(float dt)
	{
		AnimationControllerState* transitionState = controller->Transition(state);
		
		if (transitionState)
		{
			float transitionWeight = 1.0f;
			for (auto& pastState : pastStates)
				transitionWeight -= pastState.TransitionWeight;
			pastStates.push_back({ state, TimePos, transitionWeight });
			state = transitionState;
			TimePos = 0.0f;
		}

		for (auto iter = pastStates.begin(); iter != pastStates.end();)
		{
			iter->TransitionTimePost -= dt;
			if (iter->TransitionTimePost < 0.0f)
				iter = pastStates.erase(iter);
			else
			{
				iter->TimePos += dt;
				if (iter->TimePos > controller->GetClipEndTime(iter->state))
					iter->TimePos -= controller->GetClipEndTime(iter->state);
				iter->TransitionWeight = min(iter->TransitionWeight, iter->TransitionTimePost / 0.25f);
				++iter;
			}
		}

		TimePos += dt;
		if (TimePos > controller->GetClipEndTime(state))
			TimePos -= controller->GetClipEndTime(state);
		controller->GetFinalTransforms(state, TimePos, pastStates, FinalTransforms);
	}
	void SetFloat(std::string name, float value)
	{
		controller->parameters[name].Float = value;
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