#pragma once

#include "FrameResource.h"

class Graphics : public Singleton<Graphics>
{
public:
	static const UINT FrameCount{ 2 };

	ComPtr<ID3D11On12Device> device11On12;
	ComPtr<ID3D11Resource> wrappedBackBuffers[FrameCount];
	ComPtr<ID3D11DeviceContext> d11DeviceContext;

	ComPtr<ID2D1Bitmap1> renderTargets2d[FrameCount];
	ComPtr<ID2D1DeviceContext2> deviceContext;
	ComPtr<ID2D1Device2> d2dDevice;

	ComPtr<IDWriteFactory> writeFactory;
	ComPtr<ID2D1SolidColorBrush> textBrush;
	ComPtr<IDWriteTextFormat> textFormat;


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
	void RenderUI();
	void Destroy();

	//--------------//
	void InitDirect3D();
	void LoadAssets();

	void CreateDepthStencilView();

	void ChangeSwapChainState();

	void WaitForPreviousFrame();
};
