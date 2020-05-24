#pragma once

struct Keyframe
{
	float TimePos{ 0.0f };
	XMFLOAT3 Translation{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 Scale{ 1.0f, 1.0f, 1.0f };
	XMFLOAT4 RotationQuat{ 0.0f, 0.0f, 0.0f, 1.0f };
};

struct BoneAnimation
{
	std::vector<Keyframe> Keyframes;

	float GetStartTime() const { return Keyframes.front().TimePos; }
	float GetEndTime() const { return Keyframes.back().TimePos; }

	void Interpolate(float t, XMFLOAT4X4& M) const;
	void Interpolate(float t, XMFLOAT3& T, XMFLOAT3& S, XMFLOAT4& R) const;
};

struct AnimationClip
{
	std::vector<BoneAnimation> BoneAnimations;

	float GetClipStartTime() const;
	float GetClipEndTime() const;
};

enum class AnimatorControllerParameterType
{
	None = 0, Float = 1, Int = 3, Bool = 4, Trigger = 9
};

struct AnimatorControllerParameter
{
	std::string Name;
	AnimatorControllerParameterType Type{ AnimatorControllerParameterType::None };
	union
	{
		float Float{};
		int Int;
		bool Bool;
	};
};

enum AnimatorControllerStateTransitionConditionOperatorType
{
	Greater, Less, Equals, NotEqual
};

struct AnimatorControllerStateTransitionCondition
{
	std::string ParameterName;
	AnimatorControllerStateTransitionConditionOperatorType operatorType;
	union
	{
		float Float{};
		int Int;
		bool Bool;
	};

	static AnimatorControllerStateTransitionCondition CreateFloat(std::string name, 
		AnimatorControllerStateTransitionConditionOperatorType operatorType, float value)
	{
		AnimatorControllerStateTransitionCondition condition{};
		condition.ParameterName = name;
		condition.operatorType = operatorType;
		condition.Float = value;
		return condition;
	}
};
typedef AnimatorControllerStateTransitionCondition TransitionCondition;

struct AnimatorControllerStateTransition
{
	std::string Name;
	std::string DestinationStateName;
	std::vector<AnimatorControllerStateTransitionCondition> conditions;
};

struct AnimatorControllerState
{
	std::string Name;

	AnimationClip* motion;
	std::vector< AnimatorControllerStateTransition> transitionns;
};

struct PastState
{
	AnimatorControllerState* state{ nullptr };
	float TimePos{ 0.0f };
	float TransitionWeight{ 1.0f };
	float TransitionTimePost{ 0.25f };
};

class AnimatorController
{
public:
	std::vector<int> mBoneHierarchy;
	std::vector<Matrix4x4> mBoneOffsets;
	std::unordered_map<std::string, AnimatorControllerState> states;
	std::unordered_map<std::string, AnimatorControllerParameter> parameters;

public:
	UINT BoneCount() const { return mBoneHierarchy.size(); }

	float GetClipStartTime(const AnimatorControllerState* state) const { return state->motion->GetClipStartTime(); }
	float GetClipEndTime(const AnimatorControllerState* state) const { return state->motion->GetClipEndTime(); }

	void GetFinalTransforms(const AnimatorControllerState* state, float timePos,
		std::vector<PastState>& pastStates, std::vector<Matrix4x4>& finalTransforms) const;
	AnimatorControllerState* Transition(AnimatorControllerState* state);

	void AddState(std::string name, AnimationClip* clip)
	{
		states[name] = AnimatorControllerState{ name, clip };
	}
	void AddParameterFloat(std::string name, float value = 0.0f)
	{
		AnimatorControllerParameter param;
		param.Name = name;
		param.Type = AnimatorControllerParameterType::Float;
		param.Float = value;
		parameters[name] = param;
	}
	void AddTransition(std::string from, std::string to, AnimatorControllerStateTransitionCondition condition)
	{
		AnimatorControllerStateTransition transition{};
		transition.DestinationStateName = to;
		transition.conditions.push_back(condition);

		states[from].transitionns.push_back(transition);
	}
};
