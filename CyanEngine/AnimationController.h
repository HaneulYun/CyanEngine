#pragma once
#include "framework.h"
#include "SkinnedData.h"

class AnimationController : public MonoBehavior<AnimationController>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	SkinnedData* SkinnedInfo;
	std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
	std::string ClipName;
	float TimePos = 0.0f;
	ID3D12Resource* finalTransformsResource;

private:
	friend class GameObject;
	friend class MonoBehavior<AnimationController>;
	AnimationController() = default;
	AnimationController(AnimationController&) = default;

public:
	~AnimationController() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
		FinalTransforms.resize(SkinnedInfo->BoneCount());
		UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * 96) + 255) & ~255);
		finalTransformsResource = CreateBufferResource(NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, NULL);
		finalTransformsResource->Map(0, NULL, (void**)&FinalTransforms);
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		UpdateSkinnedAnimation(Time::deltaTime);
	}

	// Called every frame and increments the time position, interpolates the 
	// animations for each bone based on the current animation clip, and 
	// generates the final transforms which are ultimately set to the effect
	// for processing in the vertex shader.
	void UpdateSkinnedAnimation(float dt)
	{
		TimePos += dt;

		// Loop animation
		if (TimePos > SkinnedInfo->GetClipEndTime(ClipName))
			TimePos = 0.0f;

		// Compute the final transforms for this time position.
		SkinnedInfo->GetFinalTransforms(ClipName, TimePos, FinalTransforms);
	}
};