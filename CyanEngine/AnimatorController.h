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

enum OperatorType
{
	Greater, Less, Equals, NotEqual
};

class AnimatorController
{
public:
	struct State
	{
		struct Transition
		{
			struct Condition
			{
				std::string ParameterName;
				OperatorType operatorType;
				union
				{
					float Float{};
					int Int;
					bool Bool;
				};

				static Condition CreateFloat(std::string name, OperatorType operatorType, float value)
				{
					Condition condition{ name, operatorType, condition.Float = value };
					return condition;
				}
				static Condition CreateInt(std::string name, OperatorType operatorType, int value)
				{
					Condition condition{ name, operatorType, condition.Int = value };
					return condition;
				}
				static Condition CreateBool(std::string name, OperatorType operatorType, bool value)
				{
					Condition condition{ name, operatorType, condition.Bool = value };
					return condition;
				}
			};

			std::string Name;
			std::string DestinationStateName;
			std::vector<Condition> conditions;
		};

		std::string Name;

		AnimationClip* motion;
		std::vector<Transition> transitionns;
	};

	struct PastState
	{
		State* state{ nullptr };
		float TimePos{ 0.0f };
		float TransitionWeight{ 1.0f };
		float TransitionTimePost{ 0.25f };
	};

	struct Parameter
	{
		enum class Type
		{
			None = 0, Float = 1, Int = 3, Bool = 4, Trigger = 9
		};

		std::string Name;
		Type Type{ Type::None };
		union
		{
			float Float{};
			int Int;
			bool Bool;
		};
	};

public:
	std::vector<int> mBoneHierarchy;
	std::vector<Matrix4x4> mBoneOffsets;
	std::unordered_map<std::string, State> states;
	std::unordered_map<std::string, Parameter> parameters;

public:
	UINT BoneCount() const { return mBoneHierarchy.size(); }

	float GetClipStartTime(const State* state) const { return state->motion->GetClipStartTime(); }
	float GetClipEndTime(const State* state) const { return state->motion->GetClipEndTime(); }

	void GetFinalTransforms(const State* state, float timePos,
		std::vector<PastState>& pastStates, std::vector<Transform*>& finalTransforms) const;
	State* Transition(State* state);

	void AddState(std::string name, AnimationClip* clip)
	{
		states[name] = State{ name, clip };
	}
	void AddParameterFloat(std::string name, float value = 0.0f)
	{
		Parameter param;
		param.Name = name;
		param.Type = Parameter::Type::Float;
		param.Float = value;
		parameters[name] = param;
	}
	void AddTransition(std::string from, std::string to, State::Transition::Condition condition)
	{
		State::Transition transition{};
		transition.DestinationStateName = to;
		transition.conditions.push_back(condition);

		states[from].transitionns.push_back(transition);
	}
};

typedef AnimatorController::State::Transition::Condition TransitionCondition;
