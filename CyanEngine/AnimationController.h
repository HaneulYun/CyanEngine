#pragma once
#include "framework.h"
#include "SkinnedData.h"

class AnimationController : public MonoBehavior<AnimationController>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	SkinnedData* SkinnedInfo;
	std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
	std::string ClipName;
	float TimePos = 0.0f;
	ID3D12Resource* finalTransformsResource;
	ID3D12Resource* finalTransformsUploadResource;
	XMFLOAT4X4* Data;

private:
	friend class GameObject;
	friend class MonoBehavior<AnimationController>;
	AnimationController() = default;
	AnimationController(AnimationController&) = default;

public:
	~AnimationController() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
		FinalTransforms.resize(SkinnedInfo->BoneCount());
		UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * 96) + 255) & ~255);

		D3D12_HEAP_PROPERTIES heapProperties;
		::ZeroMemory(&heapProperties, sizeof(D3D12_HEAP_PROPERTIES));
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC d3dResourceDesc;
		::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
		d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		d3dResourceDesc.Alignment = 0;
		d3dResourceDesc.Width = ncbElementBytes;
		d3dResourceDesc.Height = 1;
		d3dResourceDesc.DepthOrArraySize = 1;
		d3dResourceDesc.MipLevels = 1;
		d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		d3dResourceDesc.SampleDesc.Count = 1;
		d3dResourceDesc.SampleDesc.Quality = 0;
		d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_RESOURCE_STATES d3dResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;

		RendererManager::Instance()->device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, d3dResourceInitialStates, NULL, __uuidof(ID3D12Resource), (void**)&finalTransformsUploadResource);
		Data = new XMFLOAT4X4[96];
		memcpy(&Data[0], FinalTransforms.data(), sizeof(XMFLOAT4X4) * 96);
		
		finalTransformsUploadResource->Map(0, nullptr, reinterpret_cast<void**>(&Data));

	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		UpdateSkinnedAnimation(Time::deltaTime);
		memcpy(&Data[0], FinalTransforms.data(), sizeof(XMFLOAT4X4) * 96);
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