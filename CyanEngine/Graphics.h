#pragma once

#include "FrameResource.h"
#include "SkinnedData.h"
#include "LoadM3d.h"

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
