#include "pch.h"
#include "RendererManager.h"

ID3D12GraphicsCommandList* RendererManager::m_pd3dCommandList = nullptr;

RendererManager::RendererManager()
{
	for (int i = 0; i < m_nSwapChainBuffers; i++)
		m_nFenceValues[i] = 0;

	m_pCamera = Camera::Instance();
	m_pCamera->SetViewport(0, 0, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight);
	m_pCamera->GenerateProjectionMatrix(1.0f, 500.0f, float(CyanWindow::m_nWndClientWidth) / float(CyanWindow::m_nWndClientHeight), 90.0f);
	m_pCamera->GenerateViewMatrix(XMFLOAT3(0.0f, 15.0f, -25.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();

	CreateRenderTargetView();
	CreateDepthStencilView();
}

RendererManager::~RendererManager()
{
}

void RendererManager::Start()
{
	for (auto& d : instances)
	{
		Shader* shader = d.first.first;
		Material* material = dynamic_cast<Renderer*>(d.second.second[0]->renderer)->material;
		shader->m_ppd3dPipelineStates = new ID3D12PipelineState*[1];
		shader->CreateShader(m_pd3dDevice, material->rootSignature);

		d.second.first = new INSTANCING();

		d.second.first->m_pd3dcbGameObjects = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, sizeof(VS_VB_INSTANCE) * d.second.second.size(), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		
		d.second.first->m_pd3dcbGameObjects->Map(0, NULL, (void**)& d.second.first->m_pcbMappedGameObjects);
		
		d.second.first->m_d3dInstancingBufferView.BufferLocation = d.second.first->m_pd3dcbGameObjects->GetGPUVirtualAddress();
		d.second.first->m_d3dInstancingBufferView.StrideInBytes = sizeof(VS_VB_INSTANCE);
		d.second.first->m_d3dInstancingBufferView.SizeInBytes = sizeof(VS_VB_INSTANCE) * d.second.second.size();
	}
}

void RendererManager::Update()
{
	for (auto& d : instances)
	{
		int j = 0;
		for (auto& gameObject : d.second.second)
		{
			d.second.first->m_pcbMappedGameObjects[j].m_xmcColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			XMStoreFloat4x4(&d.second.first->m_pcbMappedGameObjects[j].m_xmf4x4Transform, XMMatrixTranspose(XMLoadFloat4x4(&gameObject->transform->localToWorldMatrix)));
			++j;
		}
	}
}

void RendererManager::PreRender()
{
	HRESULT hResult = m_pd3dCommandAllocator->Reset(); 
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	// m_pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	// m_pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);


	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, FALSE, &d3dDsvCPUDescriptorHandle);

	//float pfClearColor[4] = { 49.0f / 256.0f, 77.0f / 256.0f, 121.0f / 256.0f, 1.0f };
	float pfClearColor[4] = { 0.1921569f, 0.3019608, 0.4745098, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor, 0, NULL);

	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
}

void RendererManager::Render()
{
	for (auto& d : instances)
	{
		//Shader* shader = d.first.first;
		Mesh* mesh = d.first.second;

		m_pd3dCommandList->SetPipelineState(d.first.first->m_ppd3dPipelineStates[0]);

		if (memcmp(&d.second.first->m_d3dInstancingBufferView, &D3D12_VERTEX_BUFFER_VIEW(), sizeof(D3D12_VERTEX_BUFFER_VIEW)))
			mesh->Render(m_pd3dCommandList, d.second.second.size(), d.second.first->m_d3dInstancingBufferView);
		else
			mesh->Render(m_pd3dCommandList, d.second.second.size());
	}
}

void RendererManager::PostRender()
{
	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	HRESULT hResult = m_pd3dCommandList->Close();

	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists), ppd3dCommandLists);

	WaitForGpuComplete();

	m_pdxgiSwapChain->Present(0, 0);

	MoveToNextFrame();

	//DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	//dxgiPresentParameters.DirtyRectsCount = 0;
	//dxgiPresentParameters.pDirtyRects = NULL;
	//dxgiPresentParameters.pScrollRect = NULL;
	//dxgiPresentParameters.pScrollOffset = NULL;
	//m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);
	//m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

void RendererManager::Destroy()
{
	::CloseHandle(m_hFenceEvent);

#if defined(_DEBUG)
	if (m_pd3dDebugController) m_pd3dDebugController->Release();
#endif

	for (int i = 0; i < m_nSwapChainBuffers; i++)
		if (m_ppd3dRenderTargetBuffers[i])
			m_ppd3dRenderTargetBuffers[i]->Release();
	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();

	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();

	if (m_pd3dFence) m_pd3dFence->Release();

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pdxgiFactory) m_pdxgiFactory->Release();
}


//--------------//

inline void RendererManager::CreateDirect3DDevice()
{
#if defined(_DEBUG)
	D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)& m_pd3dDebugController);
	m_pd3dDebugController->EnableDebugLayer();
#endif

	::CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)& m_pdxgiFactory);

	IDXGIAdapter1* pd3dAdapter = NULL;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0,
			_uuidof(ID3D12Device), (void**)& m_pd3dDevice))) break;
	}

	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void**)& pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void
			**)& m_pd3dDevice);
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x ���� ���ø�
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;

	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;
	m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)& m_pd3dFence);

	m_nFenceValues[0] = 0;
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	m_pCamera->m_d3dViewport.TopLeftX = 0;
	m_pCamera->m_d3dViewport.TopLeftY = 0;
	m_pCamera->m_d3dViewport.Width = static_cast<float>(CyanWindow::m_nWndClientWidth);
	m_pCamera->m_d3dViewport.Height = static_cast<float>(CyanWindow::m_nWndClientHeight);
	m_pCamera->m_d3dViewport.MinDepth = 0.0f;
	m_pCamera->m_d3dViewport.MaxDepth = 1.0f;

	m_pCamera->m_d3dScissorRect = { 0, 0, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight };

	if (pd3dAdapter) pd3dAdapter->Release();
}

inline void RendererManager::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void**)& m_pd3dCommandQueue);

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)& m_pd3dCommandAllocator);

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)& m_pd3dCommandList);

	hResult = m_pd3dCommandList->Close();

}

inline void RendererManager::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(CyanWindow::m_hWnd, &rcClient);
	CyanWindow::m_nWndClientWidth = rcClient.right - rcClient.left;
	CyanWindow::m_nWndClientHeight = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = CyanWindow::m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = CyanWindow::m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = CyanWindow::m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Windowed = TRUE;

	m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain * *)& m_pdxgiSwapChain);
	//m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hWnd, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1 * *)& m_pdxgiSwapChain);
	m_pdxgiFactory->MakeWindowAssociation(CyanWindow::m_hWnd, DXGI_MWA_NO_ALT_ENTER);

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	HRESULT hResult = m_pdxgiFactory->MakeWindowAssociation(CyanWindow::m_hWnd, DXGI_MWA_NO_ALT_ENTER);
#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetView();
#endif
}

inline void RendererManager::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)& m_pd3dRtvDescriptorHeap);
	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)& m_pd3dDsvDescriptorHeap);
	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

inline void RendererManager::CreateRenderTargetView()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	HRESULT hResult;
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		hResult = m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)& m_ppd3dRenderTargetBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
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
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void**)& m_pd3dDepthStencilBuffer);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
}

void RendererManager::ChangeSwapChainState()
{
	WaitForGpuComplete();

	BOOL bFullScreenState = FALSE;
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = CyanWindow::m_nWndClientWidth;
	dxgiTargetParameters.Height = CyanWindow::m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

	for (int i = 0; i < m_nSwapChainBuffers; i++);
	//	if (m_ppd3dSwapChainBackBuffers[i])
	//		m_ppd3dSwapChainBackBuffers[i]->Release();
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetView();
}

void RendererManager::WaitForGpuComplete()
{
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void RendererManager::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}