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
public:
	static const UINT FrameCount{ 2 };

	static const UINT TextureWidth{ 256 };
	static const UINT TextureHeight{ 256 };
	static const UINT TexturePixelSize{ 4 };

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
	};

	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12Resource> renderTargets[FrameCount];
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12CommandQueue> commandQueue;
	
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12DescriptorHeap> srvHeap;
	ComPtr<ID3D12PipelineState> pipelineState;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	UINT rtvDescriptorSize{ 0 };
	UINT dsvDescriptorSize{ 0 };

	ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	ComPtr<ID3D12Resource> texture;

	UINT frameIndex{ 0 };
	HANDLE fenceEvent{ nullptr };
	ComPtr<ID3D12Fence> fence;
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

	std::vector<UINT8> GenerateTextureData();

	void CreateDepthStencilView();

	void ChangeSwapChainState();

	void WaitForPreviousFrame();
};
