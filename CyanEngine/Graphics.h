#pragma once

#include "ShadowMap.h"

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
	std::vector<ComPtr<ID2D1SolidColorBrush>> textBrushes;
	std::vector<ComPtr<IDWriteTextFormat>> textFormats;

	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12Resource> renderTargets[FrameCount];
	ComPtr<ID3D12Resource> depthStencilBuffer;
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12CommandQueue> commandQueue;
	
	ComPtr<ID3D12RootSignature> rootSignature;
	HeapManager heapManager;
	//ComPtr<ID3D12DescriptorHeap> rtvHeap;
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

	std::unordered_map<std::string, ComPtr<ID3DBlob>> shaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> pipelineStates;

	UINT frameIndex{ 0 };
	ComPtr<ID3D12Fence> fence;
	UINT64 fenceValue{ 0 };
	 
private:
	bool m_bMsaa4xEnable{ false };
	UINT m_nMsaa4xQualityLevels{ 0 };

public:
	bool isShadowDebug{ false };

public:
	Graphics() {}
	virtual ~Graphics() {}

	void Initialize();

	void PreRender();
	void RenderShadowMap();
	void Render();
	void RenderObjects(int layerIndex, bool isShadowMap = false);
	void RenderUI();
	void PostRender();
	void Destroy();

	//--------------//
	void InitDirect3D();
	void InitDirect2D();
	void LoadAssets();

	void ChangeSwapChainState();

	void WaitForPreviousFrame();
	void FlushCommandQueue();

	//--------------//
	ComPtr<ID3D12Resource> diffuseMap{ nullptr };
	ComPtr<ID3D12Resource> normalMap{ nullptr };

	ComPtr<ID3D12Resource> lightDiffuse{ nullptr };
	ComPtr<ID3D12Resource> lightSpecular{ nullptr };
	void BuildResources();
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetSrv(int index) const;
	//CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtv(int index) const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsv(int index) const;

	CD3DX12_GPU_DESCRIPTOR_HANDLE GetSrvGpu(int index) const;
};
