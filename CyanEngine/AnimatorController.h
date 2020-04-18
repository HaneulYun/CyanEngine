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
};

struct AnimationClip
{
	std::vector<BoneAnimation> BoneAnimations;

	float GetClipStartTime() const;
	float GetClipEndTime() const;

	void Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& boneTransforms) const;
};

enum class AnimatorControllerParameterType
{
	None = 0, Float = 1, Int = 3, Bool = 4, Trigger = 9
};

struct AnimationControllerParameter
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

enum class AnimationControllerStateTransitionConditionOperatorType
{
	Greater, Less, Equals, NotEqual
};

struct AnimationControllerStateTransitionCondition
{
	std::string ParameterName;
	AnimationControllerStateTransitionConditionOperatorType operatorType;
	union
	{
		float Float{};
		int Int;
		bool Bool;
	};
};

struct AnimationControllerStateTransition
{
	std::string Name;

	std::string DestinationStateName;

	std::vector<AnimationControllerStateTransitionCondition> conditions;
};

struct AnimationControllerState
{
	std::string Name;

	AnimationClip* motion;
	std::vector< AnimationControllerStateTransition> transitionns;
};

class AnimatorController
{
public:
	std::vector<int> mBoneHierarchy;
	std::vector<XMFLOAT4X4> mBoneOffsets;
	std::unordered_map<std::string, AnimationControllerState> states;
	std::unordered_map<std::string, AnimationControllerParameter> parameters;

public:
	UINT BoneCount() const { return mBoneHierarchy.size(); }

	float GetClipStartTime(const AnimationControllerState* state) const { return state->motion->GetClipStartTime(); }
	float GetClipEndTime(const AnimationControllerState* state) const { return state->motion->GetClipEndTime(); }

	void GetFinalTransforms(const AnimationControllerState* state, float timePos,
		std::vector<DirectX::XMFLOAT4X4>& finalTransforms) const;
	bool Transition(AnimationControllerState*& state);

	void AddState(std::string name, AnimationClip* clip)
	{
		states[name] = AnimationControllerState{ name, clip };
	}
	void AddParameterFloat(std::string name, float value = 0.0f)
	{
		AnimationControllerParameter param;
		param.Name = name;
		param.Type = AnimatorControllerParameterType::Float;
		param.Float = value;
		parameters[name] = param;
		//parameters[name] = AnimationControllerParameter{ name, AnimatorControllerParameterType::Float, param.Float = value };
	}

};
