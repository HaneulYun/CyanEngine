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

struct AnimationState
{
	std::string Name;

	AnimationClip* motion;
};

class AnimatorController
{
public:
	std::vector<int> mBoneHierarchy;
	std::vector<XMFLOAT4X4> mBoneOffsets;
	std::unordered_map<std::string, AnimationState> states;

public:
	UINT BoneCount() const { return mBoneHierarchy.size(); }

	float GetClipStartTime(const AnimationState* state) const { return state->motion->GetClipStartTime(); }
	float GetClipEndTime(const AnimationState* state) const { return state->motion->GetClipEndTime(); }

	void GetFinalTransforms(const AnimationState* state, float timePos,
		std::vector<DirectX::XMFLOAT4X4>& finalTransforms) const;

	void AddState(std::string name, AnimationClip* clip)
	{
		states[name] = AnimationState{ name, clip };
	}
};
