#pragma once

struct MEMORY
{
	XMFLOAT4X4 transform;
	XMFLOAT4 color;
};

struct INSTANCING
{
	ID3D12Resource* resource{ nullptr };
	MEMORY* memory{ nullptr };

	Shader* shader;
};

class RendererManager : public Singleton<RendererManager>
{
private:
	static const UINT FrameCount{ 2 };

	ComPtr<IDXGISwapChain3> swapChain{ nullptr };
public:
	ComPtr<ID3D12Device> device{ nullptr };
private:
	ComPtr<ID3D12Resource> renderTargets[FrameCount];
public:
	ComPtr<ID3D12CommandAllocator> commandAllocator{ nullptr };
	ComPtr<ID3D12CommandQueue> commandQueue{ nullptr };
private:
	ComPtr<ID3D12DescriptorHeap> rtvHeap{ nullptr };
	ComPtr<ID3D12DescriptorHeap> dsvHeap{ nullptr };
public:
	ComPtr<ID3D12GraphicsCommandList> commandList{ nullptr };
private:
	UINT rtvDescriptorSize;
	UINT dsvDescriptorSize;

	UINT frameIndex{ 0 };
	HANDLE fenceEvent{ nullptr };
	ComPtr<ID3D12Fence> fence{ nullptr };
	UINT64 fenceValue{ 0 };

public:
	bool isRenewed{ false };
	void UpdateManager();

	std::map<std::pair<std::string, Mesh*>, std::pair<INSTANCING*, std::deque<GameObject*>>> instances;
	 
private:
	bool m_bMsaa4xEnable{ false };
	UINT m_nMsaa4xQualityLevels{ 0 };

	ID3D12Resource* m_pd3dDepthStencilBuffer{ nullptr };
	ID3D12PipelineState* m_pd3dPipelineState{ nullptr };

	Camera* m_pCamera{ nullptr };

public:
	RendererManager();
	virtual ~RendererManager();

	void Initialize();

	void Start();
	void Update();
	void PreRender();
	void Render();
	void InstancingRender(std::pair<std::pair<std::string, Mesh*>, std::pair<INSTANCING*, std::deque<GameObject*>>>);
	void PostRender();

	void Destroy();

	//--------------//
	void LoadPipeline();
	void LoadAssets();

	void CreateDepthStencilView();

	void ChangeSwapChainState();

	void WaitForPreviousFrame();
};
