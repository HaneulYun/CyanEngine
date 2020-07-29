#pragma once

class Animator : public MonoBehavior<Animator>
{
public:
	AnimatorController* controller;

public:
	//std::vector<Matrix4x4> FinalTransforms;
	AnimatorController::State* state{ nullptr };
	float TimePos = 0.0f;

	std::vector<AnimatorController::PastState> pastStates;

	void UpdateSkinnedAnimation(float dt, std::vector<Transform*>& finalTransforms)
	{
		AnimatorController::State* transitionState = controller->Transition(state);
		
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
				iter->TimePos = fmod(iter->TimePos + dt, controller->GetClipEndTime(iter->state));
				iter->TransitionWeight = min(iter->TransitionWeight, iter->TransitionTimePost / 0.25f);
				++iter;
			}
		}

		TimePos = fmod(TimePos + dt, controller->GetClipEndTime(state));
		controller->GetFinalTransforms(state, TimePos, pastStates, finalTransforms);
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

	void Start();
	void Update();
};