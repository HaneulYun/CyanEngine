#pragma once

#include "FrameResource.h"
#include "SkinnedData.h"
#include "LoadM3d.h"

struct SkinnedModelInstance
{
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
};

struct RenderItem
{
	RenderItem() = default;
	RenderItem(const RenderItem& rhs) = delete;

	XMFLOAT4X4 World = MathHelper::Identity4x4();
	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	int NumFramesDirty{ NUM_FRAME_RESOURCES };
	UINT ObjCBIndex{ UINT(-1) };

	Material* Mat{ nullptr };
	MeshGeometry* Geo{ nullptr };

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	UINT IndexCount{ 0 };
	UINT StartIndexLocation{ 0 };
	int BaseVertexLocation{ 0 };

	UINT SkinnedCBIndex = -1;
	SkinnedModelInstance* SkinnedModelInst = nullptr;
};

enum class RenderLayer : int
{
	Opaque = 0,
	SkinnedOpaque,
	Debug,
	Sky,
	Count
};

struct TextureData
{
	std::string name;
	std::wstring fileName;
};

class Graphics : public Singleton<Graphics>
{
public:
	static const UINT FrameCount{ 2 };

	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12Resource> renderTargets[FrameCount];
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12CommandQueue> commandQueue;
	
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12DescriptorHeap> srvHeap;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	UINT rtvDescriptorSize{ 0 };
	UINT dsvDescriptorSize{ 0 };
	union
	{
		UINT cbvDescriptorSize{ 0 };
		UINT srvDescriptorSize;
		UINT uavDescriptorSize;
	};

	static const int NumFrameResources{ NUM_FRAME_RESOURCES };
	//std::vector<std::unique_ptr<FrameResource>> frameResources;
	FrameResource* currFrameResource{ nullptr };
	int currFrameResourceIndex{ 0 };

	std::unordered_map<std::string, ComPtr<ID3DBlob>> shaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> pipelineStates;

	UINT frameIndex{ 0 };
	ComPtr<ID3D12Fence> fence;
	UINT64 fenceValue{ 0 };
	 
private:
	bool m_bMsaa4xEnable{ false };
	UINT m_nMsaa4xQualityLevels{ 0 };

	ID3D12Resource* m_pd3dDepthStencilBuffer{ nullptr };

	Camera* m_pCamera{ nullptr };

public:
	Graphics();
	virtual ~Graphics();

	void Initialize();

	void Start();
	void Update(std::vector<std::unique_ptr<FrameResource>>& frameResources);
	void PreRender();
	void Render();
	void PostRender();

	void Destroy();

	//--------------//
	void InitDirect3D();
	void LoadAssets();

	void CreateDepthStencilView();

	void ChangeSwapChainState();

	void WaitForPreviousFrame();
};
