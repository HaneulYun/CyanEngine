#pragma once

struct INSTANCING
{
	ID3D12Resource* m_pd3dcbGameObjects = NULL;
	VS_VB_INSTANCE* m_pcbMappedGameObjects = NULL;

	D3D12_VERTEX_BUFFER_VIEW m_d3dInstancingBufferView;
};

class RendererManager : public Singleton<RendererManager>
{
private:
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debug{ nullptr };
#endif
	ComPtr<IDXGIFactory4> factory{ nullptr };
	ComPtr<IDXGISwapChain3> swapChain{ nullptr };
public:
	ComPtr<ID3D12Device> device{ nullptr };
	ComPtr<ID3D12CommandQueue> commandQueue{ nullptr };
	ComPtr<ID3D12CommandAllocator> commandAllocator{ nullptr };
	ComPtr<ID3D12GraphicsCommandList> commandList{ nullptr };
private:
	ComPtr<ID3D12DescriptorHeap> rtvHeap{ nullptr };
	ComPtr<ID3D12DescriptorHeap> dsvHeap{ nullptr };

public:
	std::map<std::pair<Shader*, Mesh*>, std::pair<INSTANCING*, std::deque<GameObject*>>> instances;
	 

private:
	bool m_bMsaa4xEnable{ false };
	UINT m_nMsaa4xQualityLevels{ 0 };

	static const UINT m_nSwapChainBuffers{ 2 };
	//m_ppd3dSwapChainBackBuffers;

	UINT m_nSwapChainBufferIndex{ NULL };

	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	UINT m_nRtvDescriptorIncrementSize{ 0 };

	ID3D12Resource* m_pd3dDepthStencilBuffer{ nullptr };
	UINT m_nDsvDescriptorIncrementSize{ 0 };

private:

	ID3D12PipelineState* m_pd3dPipelineState{ nullptr };

	ComPtr<ID3D12Fence> fence{ nullptr };
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent{ nullptr };

	Camera* m_pCamera{ nullptr };


public:
	RendererManager();
	virtual ~RendererManager();

	void Start();

	void Update();
	void PreRender();
	void Render();
	void PostRender();

	void Destroy();

	//--------------//
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateSwapChain();

	void CreateRenderTargetView();
	void CreateDepthStencilView();

	void ChangeSwapChainState();

	void WaitForGpuComplete();

	void MoveToNextFrame();
};