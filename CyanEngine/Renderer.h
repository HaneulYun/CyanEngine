#pragma once

class Renderer : public Component,
	public Singleton<Renderer>
{
private:
	RendererManager* rendererManager;

public:
	// 扩档快 备己 夸家, 盒府且 鞘夸 乐澜
	HINSTANCE m_hInstance{ nullptr };
	HWND m_hWnd{ nullptr };

	int m_nWndClientWidth{ FRAME_BUFFER_WIDTH };
	int m_nWndClientHeight{ FRAME_BUFFER_HEIGHT };

private:
	// DXGI 备己 夸家
	IDXGIFactory4* m_pdxgiFactory{ nullptr };
	IDXGISwapChain3* m_pdxgiSwapChain{ nullptr };

public:
	// DX 备己 夸家
	ID3D12Device* m_pd3dDevice{ nullptr };

private:
	bool m_bMsaa4xEnable{ false };
	UINT m_nMsaa4xQualityLevels{ 0 };

	static const UINT m_nSwapChainBuffers{ 2 };
	//m_ppd3dSwapChainBackBuffers;

	UINT m_nSwapChainBufferIndex{ NULL };

	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap{ nullptr };
	UINT m_nRtvDescriptorIncrementSize{ 0 };

	ID3D12Resource* m_pd3dDepthStencilBuffer{ nullptr };
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap{ nullptr };
	UINT m_nDsvDescriptorIncrementSize{ 0 };

public:
	ID3D12CommandQueue* m_pd3dCommandQueue{ nullptr };
	ID3D12CommandAllocator* m_pd3dCommandAllocator{ nullptr };
	ID3D12GraphicsCommandList* m_pd3dCommandList{ nullptr };
private:

	ID3D12PipelineState* m_pd3dPipelineState{ nullptr };

	ID3D12Fence* m_pd3dFence{ nullptr };
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent{ nullptr };

#if defined(_DEBUG)
	ID3D12Debug* m_pd3dDebugController{ nullptr };
#endif
	Camera* m_pCamera{ nullptr };

public:
	Renderer();
	~Renderer();

	void OnStart();

	void Update();
	void PreRender();
	void Render();
	void PostRender();

	void OnDestroy();

	//--------------//
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	void CreateSwapChain();

	void CreateRtvAndDsvDescriptorHeaps();
	void CreateRenderTargetView();
	void CreateDepthStencilView();

	void ChangeSwapChainState();

	void WaitForGpuComplete();

	void MoveToNextFrame();
};