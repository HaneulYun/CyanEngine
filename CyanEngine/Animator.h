#pragma once
#include "SkinnedData.h"

//struct SkinnedModelInstance
//{
//	SkinnedData* SkinnedInfo = nullptr;
//	std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
//	std::string ClipName;
//	float TimePos = 0.0f;
//
//	void UpdateSkinnedAnimation(float dt)
//	{
//		TimePos += dt;
//
//		float k = SkinnedInfo->GetClipEndTime(ClipName);
//
//		if (TimePos > SkinnedInfo->GetClipEndTime(ClipName))
//			TimePos = 0.0f;
//
//		SkinnedInfo->GetFinalTransforms(ClipName, TimePos, FinalTransforms);
//	}
//};

class Animator : public MonoBehavior<Animator>
{
public:
	SkinnedData* SkinnedInfo = nullptr;
	std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
	std::string ClipName;
	float TimePos = 0.0f;

	void UpdateSkinnedAnimation(float dt)
	{
		TimePos += dt;

		float k = SkinnedInfo->GetClipEndTime(ClipName);

		if (TimePos > SkinnedInfo->GetClipEndTime(ClipName))
			TimePos = 0.0f;

		SkinnedInfo->GetFinalTransforms(ClipName, TimePos, FinalTransforms);
	}

	UINT SkinnedCBIndex = -1;

private:
	friend class GameObject;
	friend class MonoBehavior<Animator>;
	Animator() = default;
	Animator(Animator&) = default;

public:
	~Animator() {}

	void Start() {}
	void Update() {}
};