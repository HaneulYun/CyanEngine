#include "pch.h"
#include "RendererManager.h"

extern UINT gnCbvSrvDescriptorIncrementSize;

RendererManager::RendererManager()
{
	for (int i = 0; i < m_nSwapChainBuffers; i++)
		m_nFenceValues[i] = 0;


	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();

	CreateRenderTargetView();
	CreateDepthStencilView();

	commandList->Reset(commandAllocator.Get(), NULL);
}

RendererManager::~RendererManager()
{
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

	for (auto& d : instances)
	{
		if (!d.second.first)
		{
			d.second.first = new INSTANCING();
			Shader* shader = d.second.first->shader = dynamic_cast<Renderer*>(d.second.second[0]->renderer)->material->shader;

			//Material* material = dynamic_cast<Renderer*>(d.second.second[0]->renderer)->material;
			shader->rootSignature = shader->CreateGraphicsRootSignature(device.Get());
			shader->CreateShader(device.Get(), shader->rootSignature);

		}

		UINT ncbElementBytes = ((sizeof(MEMORY) + 255) & ~255);
		d.second.first->resource = CreateBufferResource(NULL, ncbElementBytes * d.second.second.size(), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

		d.second.first->resource->Map(0, NULL, (void**)&d.second.first->memory);

		if (typeid(*d.second.first->shader).name() == typeid(TextureShader).name() ||
			typeid(*d.second.first->shader).name() == typeid(CSkyBoxShader).name())
		{
			//d.second.first->shader->CreateConstantBufferViews(d.second.second.size(), d.second.first->resource, ncbElementBytes);
		}
	}

	//commandList->Close();
	//ID3D12CommandList* ppd3dCommandLists[] = { commandList.Get() };
	//commandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	//WaitForGpuComplete();
}

void RendererManager::Update()
{
	for (auto& d : instances)
	{
		int j = 0;
		commandList->SetGraphicsRootShaderResourceView(2, d.second.first->resource->GetGPUVirtualAddress());
		for (auto& gameObject : d.second.second)
		{
			d.second.first->memory[j].color = dynamic_cast<Renderer*>(gameObject->renderer)->material->albedo;// XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

			XMStoreFloat4x4(&d.second.first->memory[j].transform, XMMatrixTranspose(XMLoadFloat4x4(&gameObject->GetMatrix())));
			++j;
		}
		//for (int i = 0; i < m_nTextures; i++)
		//{
		//	commandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[i].m_nRootParameterIndex, m_pRootArgumentInfos[i].m_d3dSrvGpuDescriptorHandle);
		//}
	}
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
	//Shader* shader = d.first.first;
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
		typeid(*d.second.first->shader).name() == typeid(CSkyBoxShader).name())
	{
		TextureShader* shader = dynamic_cast<TextureShader*>(d.second.first->shader);
		commandList->SetDescriptorHeaps(1, &d.second.first->shader->m_pd3dCbvSrvDescriptorHeap);
		for (int i = 0; i < TEXTURES; i++)
		{
			commandList->SetGraphicsRootDescriptorTable(shader->ppMaterials[i]->m_pTexture->m_pRootArgumentInfos[0].m_nRootParameterIndex, shader->ppMaterials[i]->m_pTexture->m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
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


//--------------//

inline void RendererManager::CreateDirect3DDevice()
{
#if defined(_DEBUG)
	D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	debug->EnableDebugLayer();
#endif

	CreateDXGIFactory1(IID_PPV_ARGS(&factory));

	ComPtr<IDXGIAdapter1> adapter{ nullptr };
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &adapter); i++)
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		adapter->GetDesc1(&adapterDesc);
		if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device))))
			break;
	}
	if (!adapter)
	{
		factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
		D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x ´ÙÁß »ùÇÃ¸µ
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;

	device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&fence);

	m_nFenceValues[0] = 0;
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	gnCbvSrvDescriptorIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

inline void RendererManager::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	ZeroMemory(&commandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
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

inline void RendererManager::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(CyanWindow::m_hWnd, &rcClient);
	CyanWindow::m_nWndClientWidth = rcClient.right - rcClient.left;
	CyanWindow::m_nWndClientHeight = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = m_nSwapChainBuffers;
	swapChainDesc.BufferDesc.Width = CyanWindow::m_nWndClientWidth;
	swapChainDesc.BufferDesc.Height = CyanWindow::m_nWndClientHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.OutputWindow = CyanWindow::m_hWnd;
	swapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDesc;
	ZeroMemory(&swapChainFullScreenDesc, sizeof(swapChainFullScreenDesc));
	swapChainFullScreenDesc.RefreshRate.Numerator = 60;
	swapChainFullScreenDesc.RefreshRate.Denominator = 1;
	swapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainFullScreenDesc.Windowed = TRUE;

	//m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hWnd, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1 * *)& m_pdxgiSwapChain);
	factory->CreateSwapChain(commandQueue.Get(), &swapChainDesc, (IDXGISwapChain**)swapChain.GetAddressOf());
	factory->MakeWindowAssociation(CyanWindow::m_hWnd, DXGI_MWA_NO_ALT_ENTER);

	m_nSwapChainBufferIndex = swapChain->GetCurrentBackBufferIndex();

	//HRESULT hResult;
	factory->MakeWindowAssociation(CyanWindow::m_hWnd, DXGI_MWA_NO_ALT_ENTER);
#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetView();
#endif
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
	d3dResourceDesc.Width = CyanWindow::m_nWndClientWidth;
	d3dResourceDesc.Height = CyanWindow::m_nWndClientHeight;
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
	dxgiTargetParameters.Width = CyanWindow::m_nWndClientWidth;
	dxgiTargetParameters.Height = CyanWindow::m_nWndClientHeight;
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
	swapChain->ResizeBuffers(m_nSwapChainBuffers, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

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