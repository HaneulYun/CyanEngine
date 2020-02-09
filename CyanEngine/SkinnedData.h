// �ִϸ��̼��� ���� �����Ͱ� ����Ǿ� ����
// - Bone Animation�� ���� Skeleton ���� (ParentIndex, OffsetMtx)
// - AnimationSets

#pragma once
#include "framework.h"
#include <unordered_map>

#define FLT_MAX          3.402823466e+38F        // max value

struct Keyframe
{
	Keyframe()
		: TimePos(0.0f),
		Translation(0.0f, 0.0f, 0.0f),
		Scale(1.0f, 1.0f, 1.0f),
		RotationQuat(0.0f, 0.0f, 0.0f, 1.0f)
	{
	};
	~Keyframe() {};

	// Ű �������� �ð� (��)
	float TimePos;
	DirectX::XMFLOAT3 Translation;
	DirectX::XMFLOAT3 Scale;
	DirectX::XMFLOAT4 RotationQuat;
};


struct BoneAnimation
{
	float GetStartTime()const;

	float GetEndTime()const;

	void Interpolate(float t, DirectX::XMFLOAT4X4& M)const;

	std::vector<Keyframe> Keyframes;
};

///<summary>
/// Examples of AnimationClips are "Walk", "Run", "Attack", "Defend".
/// An AnimationClip requires a BoneAnimation for every bone to form
/// the animation clip.    
///</summary>
struct AnimationClip
{
	float GetClipStartTime()const;

	float GetClipEndTime()const;

	void Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& boneTransforms)const;

	std::vector<BoneAnimation> BoneAnimations;
};

//�ѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤ�

class SkinnedData
{
private:
	// Gives parentIndex of ith bone.
	std::vector<int> mBoneHierarchy; // [1]��° �� = 0 ��.

	std::vector<DirectX::XMFLOAT4X4> mBoneOffsets;	// [0] ��° ���� Offset ��� = 

	std::unordered_map<std::string, AnimationClip> mAnimations; // �� ���� Animation��

public:

private:

public:
	SkinnedData() {}
	~SkinnedData() {}

	UINT BoneCount()const;

	float GetClipStartTime(const std::string& clipName)const;
	float GetClipEndTime(const std::string& clipName)const;


	void Set(
		std::vector<int>& boneHierarchy,
		std::vector<DirectX::XMFLOAT4X4>& boneOffsets,
		std::unordered_map<std::string, AnimationClip>& animations);

	// In a real project, you'd want to cache the result if there was a chance
	// that you were calling this several times with the same clipName at 
	// the same timePos.
	void GetFinalTransforms(const std::string& clipName, float timePos,
		std::vector<DirectX::XMFLOAT4X4>& finalTransforms)const;

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};