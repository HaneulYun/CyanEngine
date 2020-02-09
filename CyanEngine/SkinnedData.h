// 애니메이션을 위한 데이터가 저장되어 있음
// - Bone Animation을 위한 Skeleton 정보 (ParentIndex, OffsetMtx)
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

	// 키 프레임의 시간 (초)
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

//ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ

class SkinnedData
{
private:
	// Gives parentIndex of ith bone.
	std::vector<int> mBoneHierarchy; // [1]번째 본 = 0 값.

	std::vector<DirectX::XMFLOAT4X4> mBoneOffsets;	// [0] 번째 본의 Offset 행렬 = 

	std::unordered_map<std::string, AnimationClip> mAnimations; // 이 본의 Animation들

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

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};