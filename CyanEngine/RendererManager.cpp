#include "pch.h"
#include "RendererManager.h"

extern UINT gnCbvSrvDescriptorIncrementSize;

RendererManager::RendererManager()
{
}

RendererManager::~RendererManager()
{
}

void RendererManager::Initialize()
{
	LoadPipeline();
	LoadAssets();

	for (int i = 0; i < m_nSwapChainBuffers; i++)
		m_nFenceValues[i] = 0;

	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();

	CreateRenderTargetView();
	CreateDepthStencilView();
}

void RendererManager::UpdateManager()
{
	if (!isRenewed)
	{
		isRenewed = true;
		Start();
	}
	//Update();
}

void RendererManager::Start()
{
	//static CTerrainShader tshader{ nullptr };
	for (auto& d : instances)
	{
		if (!d.second.first)
		{
			d.second.first = new INSTANCING();
			Shader* shader = d.second.first->shader = dynamic_cast<Renderer*>(d.second.second[0]->renderer)->material->shader;

			if (!shader->rootSignature)
				shader->rootSignature = shader->CreateGraphicsRootSignature(device.Get());
			if (!shader->pipelineState)
				shader->CreateShader();
		}
		UINT ncbElementBytes = ((sizeof(MEMORY) + 255) & ~255);

		if (d.second.first->resource)
		{
			d.second.first->resource->Unmap(0, NULL);
			d.second.first->resource->Release();
		}
		d.second.first->resource = CreateBufferResource(NULL, ncbElementBytes * d.second.second.size(), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

		d.second.first->resource->Map(0, NULL, (void**)&d.second.first->memory);
	}
}

void RendererManager::Update()
{
}

void RendererManager::PreRender()
{
	commandAllocator->Reset();
	commandList->Reset(commandAllocator.Get(), NULL);

	D3D12_RESOURCE_BARRIER resourceBarrier;
	::ZeroMemory(&resourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier.Transition.pResource = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &resourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuDescriptorHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvCpuDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);

	D3D12_CPU_DESCRIPTOR_HANDLE dsvCpuDescriptorHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();

	float clearColor[4] = { 0.1921569, 0.3019608, 0.4745098, 1.0f };
	D3D12_CLEAR_FLAGS clearFlags{ D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL };

	commandList->OMSetRenderTargets(1, &rtvCpuDescriptorHandle, FALSE, &dsvCpuDescriptorHandle);
	commandList->ClearRenderTargetView(rtvCpuDescriptorHandle, clearColor, 0, NULL);
	commandList->ClearDepthStencilView(dsvCpuDescriptorHandle, clearFlags, 1.0f, 0, 0, NULL);
}

void RendererManager::Render()
{
	PreRender();

	Camera::main->SetViewportsAndScissorRects(commandList.Get());

	for (auto& d : instances)
	{
		if (typeid(*d.second.first->shader).name() == typeid(CSkyBoxShader).name())
			InstancingRender(d);
	}
	for (auto& d : instances)
	{
		if (typeid(*d.second.first->shader).name() != typeid(CSkyBoxShader).name())
			InstancingRender(d);
	}

	PostRender();
}

void RendererManager::InstancingRender(std::pair<std::pair<std::string, Mesh*>, std::pair<INSTANCING*, std::deque<GameObject*>>> d)
{
	Mesh* mesh = d.first.second;

	commandList->SetGraphicsRootSignature(d.second.first->shader->rootSignature);
	commandList->SetPipelineState(d.second.first->shader->pipelineState.Get());

	Camera::main->UpdateShaderVariables(commandList.Get());

	int j = 0;
	commandList->SetGraphicsRootShaderResourceView(2, d.second.first->resource->GetGPUVirtualAddress());
	if (typeid(*d.second.first->shader).name() == typeid(CSkyBoxShader).name())
		for (auto& gameObject : d.second.second)
		{
			d.second.first->memory[j].color = dynamic_cast<Renderer*>(gameObject->renderer)->material->albedo;
			XMFLOAT4X4 xmf4x4 = gameObject->transform->localToWorldMatrix;
			xmf4x4._41 = Camera::main->_pos.x;
			xmf4x4._42 = Camera::main->_pos.y;
			xmf4x4._43 = Camera::main->_pos.z;
			XMStoreFloat4x4(&d.second.first->memory[j].transform, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4)));
			++j;
		}
	else
		for (auto& gameObject : d.second.second)
		{
			d.second.first->memory[j].color = dynamic_cast<Renderer*>(gameObject->renderer)->material->albedo;
			XMStoreFloat4x4(&d.second.first->memory[j].transform, XMMatrixTranspose(XMLoadFloat4x4(&gameObject->GetMatrix())));
			++j;
		}

	if (typeid(*d.second.first->shader).name() == typeid(TextureShader).name() ||
		typeid(*d.second.first->shader).name() == typeid(CBillboardObjectsShader).name() ||
		typeid(*d.second.first->shader).name() == typeid(CSkyBoxShader).name() ||
		typeid(*d.second.first->shader).name() == typeid(CTerrainShader).name())
	{
		TextureShader* shader = dynamic_cast<TextureShader*>(d.second.first->shader);

		if (typeid(*d.second.first->shader).name() == typeid(CTerrainShader).name())
			int k = 0;

		commandList->SetDescriptorHeaps(1, &d.second.first->shader->m_pd3dCbvSrvDescriptorHeap);
		for (int i = 0; i < TEXTURES; i++)
		{
			shader->ppMaterials[i]->m_pTexture->UpdateShaderVariables(commandList.Get());
			//commandList->SetGraphicsRootDescriptorTable(shader->ppMaterials[i]->m_pTexture->m_pRootArgumentInfos[0].m_nRootParameterIndex, shader->ppMaterials[i]->m_pTexture->m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
		}
	}
	
	if (typeid(*mesh).name() == typeid(CMeshIlluminatedFromFile).name())
		((CMeshIlluminatedFromFile*)mesh)->Render(d.second.second.size(), 0);
	else
		mesh->Render(d.second.second.size());
}

void RendererManager::PostRender()
{
	D3D12_RESOURCE_BARRIER resourceBarrier;
	::ZeroMemory(&resourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier.Transition.pResource = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &resourceBarrier);
	commandList->Close();

	ID3D12CommandList* ppd3dCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists), ppd3dCommandLists);

	WaitForGpuComplete();

	swapChain->Present(0, 0);

	MoveToNextFrame();
}

void RendererManager::Destroy()
{
	::CloseHandle(m_hFenceEvent);

	for (int i = 0; i < m_nSwapChainBuffers; i++)
		if (m_ppd3dRenderTargetBuffers[i])
			m_ppd3dRenderTargetBuffers[i]->Release();

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();

	swapChain->SetFullscreenState(FALSE, NULL);
}

void RendererManager::LoadPipeline()
{
	UINT dxgiFactoryFlags{ 0 };

#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debug{ nullptr };
	D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	debug->EnableDebugLayer();

	dxgiFactoryFlags != DXGI_CREATE_FACTORY_DEBUG;
#endif

	ComPtr<IDXGIFactory4> factory{ nullptr };
	CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));

	ComPtr<IDXGIAdapter1> adapter{ nullptr };
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &adapter); ++i)
	{
		DXGI_ADAPTER_DESC1 desc{};
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device))))
			break;
	}
	if (!adapter)
	{
		factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
		D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
	}

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.BufferCount = frameCount;
	swapChainDesc.Width = CyanFW::Instance()->GetWidth();
	swapChainDesc.Height = CyanFW::Instance()->GetHeight();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	factory->CreateSwapChainForHwnd(commandQueue.Get(), CyanApp::GetHwnd(), &swapChainDesc, nullptr, nullptr, (IDXGISwapChain1**)swapChain.GetAddressOf() );
	factory->MakeWindowAssociation(CyanApp::GetHwnd(), DXGI_MWA_NO_ALT_ENTER);
	frameIndex = swapChain->GetCurrentBackBufferIndex();

	// D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	// d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x ´ÙÁß »ùÇÃ¸µ
	// d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	// d3dMsaaQualityLevels.NumQualityLevels = 0;
	// 
	// device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	// m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	// m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&fence);
	
	m_nFenceValues[0] = 0;
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	
	gnCbvSrvDescriptorIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void RendererManager::LoadAssets()
{
}


//--------------//

inline void RendererManager::CreateCommandQueueAndList()
{
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), NULL, IID_PPV_ARGS(&commandList));

	commandList->Close();
}

inline void RendererManager::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
	::ZeroMemory(&descriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	descriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeapDesc.NodeMask = 0;
	//HRESULT hResult;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&rtvHeap));
	m_nRtvDescriptorIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&dsvHeap));
	m_nDsvDescriptorIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

inline void RendererManager::CreateRenderTargetView()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();

	HRESULT hResult;
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		hResult = swapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_ppd3dRenderTargetBuffers[i]);
		device->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

inline void RendererManager::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = CyanFW::Instance()->GetWidth();
	d3dResourceDesc.Height = CyanFW::Instance()->GetHeight();;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;
	device->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	device->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
}

void RendererManager::ChangeSwapChainState()
{
	WaitForGpuComplete();

	BOOL bFullScreenState = FALSE;
	swapChain->GetFullscreenState(&bFullScreenState, NULL);
	swapChain->SetFullscreenState(!bFullScreenState, NULL);

	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = CyanFW::Instance()->GetWidth();
	dxgiTargetParameters.Height = CyanFW::Instance()->GetHeight();;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChain->ResizeTarget(&dxgiTargetParameters);

	for (int i = 0; i < m_nSwapChainBuffers; i++);
	//	if (m_ppd3dSwapChainBackBuffers[i])
	//		m_ppd3dSwapChainBackBuffers[i]->Release();
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	swapChain->GetDesc(&dxgiSwapChainDesc);
	swapChain->ResizeBuffers(m_nSwapChainBuffers, CyanFW::Instance()->GetWidth(), CyanFW::Instance()->GetHeight(), dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	m_nSwapChainBufferIndex = swapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetView();
}

void RendererManager::WaitForGpuComplete()
{
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = commandQueue->Signal(fence.Get(), nFenceValue);
	if (fence->GetCompletedValue() < nFenceValue)
	{
		hResult = fence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void RendererManager::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = swapChain->GetCurrentBackBufferIndex();

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = commandQueue->Signal(fence.Get(), nFenceValue);
	if (fence->GetCompletedValue() < nFenceValue)
	{
		hResult = fence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}