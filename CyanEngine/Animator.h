#pragma once

class Animator : public MonoBehavior<Animator>
{
public:
	AnimatorController* controller;

public:
	std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
	AnimationControllerState* state{ nullptr };
	float TimePos = 0.0f;

	AnimationControllerState* nextState{ nullptr };
	float nextTimePos = 0.0f;
	float transitionTime = 0.0f;

	void UpdateSkinnedAnimation(float dt)
	{
		AnimationControllerState* transitionState;
		if(nextState)
			transitionState = controller->Transition(nextState);
		else
			transitionState = controller->Transition(state);

		if (transitionState)
		{
			Debug::Log((transitionState->Name + "\n").c_str());
			if (nextState)
				state = nextState;
			nextState = transitionState;
			transitionTime = 0.0;
		}

		float lerpPercent = 0;
		if (nextState)
		{
			nextTimePos += dt;
			transitionTime += dt;
			if (nextTimePos > controller->GetClipEndTime(nextState))
				nextTimePos -= controller->GetClipEndTime(nextState);

			lerpPercent = transitionTime / 0.25;
		}
		if (lerpPercent > 1)
		{
			state = nextState;
			nextState = nullptr;
			
			TimePos = nextTimePos;
			nextTimePos = 0;
			
			transitionTime = 0;
		}

		TimePos += dt;
		if (TimePos > controller->GetClipEndTime(state))
			TimePos -= controller->GetClipEndTime(state);
		controller->GetFinalTransforms(state, TimePos, 
			nextState, nextTimePos, lerpPercent, FinalTransforms);
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