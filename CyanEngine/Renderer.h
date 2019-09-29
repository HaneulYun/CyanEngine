#pragma once

class Renderer : public Component,
	public Singleton<Renderer>
{
public:
	// ������ ���� ���, �и��� �ʿ� ����
	HINSTANCE m_hInstance{ nullptr };
	HWND m_hWnd{ nullptr };

	int m_nWndClientWidth{ FRAME_BUFFER_WIDTH };
	int m_nWndClientHeight{ FRAME_BUFFER_HEIGHT };

private:
	// DXGI ���� ���
	IDXGIFactory4* m_pdxgiFactory{ nullptr };
	IDXGISwapChain3* m_pdxgiSwapChain{ nullptr };

	// DX ���� ���
	ID3D12Device* m_pd3dDevice{ nullptr };

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

	ID3D12CommandQueue* m_pd3dCommandQueue{ nullptr };
	ID3D12CommandAllocator* m_pd3dCommandAllocator{ nullptr };
	ID3D12GraphicsCommandList* m_pd3dCommandList{ nullptr };

	ID3D12PipelineState* m_pd3dPipelineState{ nullptr };

	ID3D12Fence* m_pd3dFence{ nullptr };
	UINT64 m_nFenceValue{ 0 };
	HANDLE m_hFenceEvent{ nullptr };

#if defined(_DEBUG)
	ID3D12Debug* m_pd3dDebugController{ nullptr };
#endif
	D3D12_VIEWPORT m_d3dViewport{ NULL };
	D3D12_RECT m_d3dScissorRect{ NULL };

	// Time
	Time m_time;
	_TCHAR m_pszFrameRate[50];

public:
	Renderer();
	~Renderer();

	void OnStart();

	void Update();
	void Render();

	void OnDestroy();

	//--------------//
	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateCommandQueueAndList();
	void CreateRenderTargetView();
	void CreateDepthStencilView();

	void ChangeSwapChainState();

	void WaitForGpuComplete();
};