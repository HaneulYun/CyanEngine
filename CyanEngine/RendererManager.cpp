#include "pch.h"
#include "RendererManager.h"

extern UINT gnCbvSrvDescriptorIncrementSize;

RendererManager::RendererManager()
{
	constantBufferData.WorldViewProj = NS_Matrix4x4::Identity();
}

RendererManager::~RendererManager()
{
}

void RendererManager::Initialize()
{
	LoadPipeline();
	LoadAssets();

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
	const XMFLOAT3 axis{ 0.0f, 1.0f, 0.0f };
	const float speedRotating{ 180.0f };

	//XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&axis), XMConvertToRadians(speedRotating * Time::deltaTime));
	//constantBufferData.WorldViewProj = NS_Matrix4x4::Multiply(mtxRotate, constantBufferData.WorldViewProj);

	XMMATRIX mWorld = XMLoadFloat4x4(&NS_Matrix4x4::Identity());

	XMMATRIX world = XMLoadFloat4x4(&Camera::main->gameObject->GetMatrix());

	XMVECTOR pos = XMVector3Transform(XMLoadFloat3(&Camera::main->pos), world);
	XMVECTOR lookAt = XMVector3Transform(XMLoadFloat3(&Camera::main->lookAt), world);
	XMVECTOR up{ 0, 1, 0 };

	XMMATRIX view = XMMatrixLookAtLH(pos, lookAt, up);
	XMMATRIX proj = XMLoadFloat4x4(&Camera::main->projection);
	XMMATRIX worldViewProj = mWorld * view * proj;
	XMStoreFloat4x4(&constantBufferData.WorldViewProj, XMMatrixTranspose(worldViewProj));

	memcpy(cbvDataBegin, &constantBufferData, sizeof(constantBufferData));
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
	commandList->Reset(commandAllocator.Get(), pipelineState.Get());

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle{ dsvHeap->GetCPUDescriptorHandleForHeapStart() };
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	const float clearColor[] { 0.1921569, 0.3019608, 0.4745098, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	D3D12_CLEAR_FLAGS clearFlags{ D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL };
	commandList->ClearDepthStencilView(dsvHandle, clearFlags, 1.0f, 0, 0, nullptr);
}

void RendererManager::Render()
{
	PreRender();

	Camera::main->SetViewportsAndScissorRects(commandList.Get());

	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetPipelineState(pipelineState.Get());
	ID3D12DescriptorHeap* heaps[]{ cbvHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(heaps), heaps);
	commandList->SetGraphicsRootDescriptorTable(0, cbvHeap->GetGPUDescriptorHandleForHeapStart());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &(triangle->VertexBufferView()));
	commandList->DrawInstanced(3, 1, 0, 0);

	commandList->IASetVertexBuffers(0, 1, &(box->VertexBufferView()));
	commandList->DrawInstanced(3, 1, 0, 0);
	commandList->IASetIndexBuffer(&box->IndexBufferView());
	commandList->DrawIndexedInstanced(box->DrawArgs["box"].IndexCount, 1, 0, 0, 0);

	//for (auto& d : instances)
	//{
	//	if (typeid(*d.second.first->shader).name() == typeid(CSkyBoxShader).name())
	//		InstancingRender(d);
	//}
	//for (auto& d : instances)
	//{
	//	if (typeid(*d.second.first->shader).name() != typeid(CSkyBoxShader).name())
	//		InstancingRender(d);
	//}

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
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	commandList->Close();

	ID3D12CommandList* ppd3dCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists), ppd3dCommandLists);

	swapChain->Present(0, 0);

	WaitForPreviousFrame();
}

void RendererManager::Destroy()
{
	WaitForPreviousFrame();

	CloseHandle(fenceEvent);

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
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = CyanFW::Instance()->GetWidth();
	swapChainDesc.Height = CyanFW::Instance()->GetHeight();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	factory->CreateSwapChainForHwnd(commandQueue.Get(), CyanApp::GetHwnd(), &swapChainDesc, nullptr, nullptr, (IDXGISwapChain1**)swapChain.GetAddressOf() );
	factory->MakeWindowAssociation(CyanApp::GetHwnd(), DXGI_MWA_NO_ALT_ENTER);
	frameIndex = swapChain->GetCurrentBackBufferIndex();


	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.NumDescriptors = FrameCount;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&rtvHeap));
	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&dsvHeap));
	dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&cbvHeap));


	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT i = 0; i < FrameCount; ++i)
	{
		swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
		device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, rtvDescriptorSize);
	}


	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
}

void RendererManager::LoadAssets()
{
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
	CD3DX12_ROOT_PARAMETER1 rootParameters[1];

	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags{
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS
	};

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error);
	device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;
#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	D3DCompileFromFile(L"..\\CyanEngine\\shaders\\shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
	D3DCompileFromFile(L"..\\CyanEngine\\shaders\\shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	CD3DX12_RASTERIZER_DESC raterizerDesc(D3D12_DEFAULT);
	raterizerDesc.CullMode = D3D12_CULL_MODE_BACK;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
	pipelineStateDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	pipelineStateDesc.pRootSignature = rootSignature.Get();
	pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	pipelineStateDesc.RasterizerState = raterizerDesc;
	pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
	pipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.NumRenderTargets = 1;
	pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineStateDesc.SampleDesc.Count = 1;
	device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState));


	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));


	float aspectRatio = CyanFW::Instance()->GetAspectRatio();
	{
		Vertex vertices[]
		{
			{ { 0.0f, 2.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 2.0f, -2.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -2.0f, -2.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		const UINT vertexBufferSize = sizeof(vertices);

		triangle = new MeshGeometry();
		D3DCreateBlob(vertexBufferSize, &triangle->VertexBufferCPU);
		CopyMemory(triangle->VertexBufferCPU->GetBufferPointer(), vertices, vertexBufferSize);

		triangle->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(),
			vertices, vertexBufferSize, triangle->VertexBufferUploader);

		triangle->VertexByteStride = sizeof(Vertex);
		triangle->VertexBufferByteSize = vertexBufferSize;
	}

	{
		Vertex vertices[]
		{
			Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
			Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
			Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
			Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
			Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
			Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
			Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
			Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
		};
		uint16_t indices[]
		{
			0, 1, 2, 0, 2, 3,	4, 6, 5, 4, 7, 6,
			4, 5, 1, 4, 1, 0,	3, 2, 6, 3, 6, 7,
			1, 5, 6, 1, 6, 2,	4, 0, 3, 4, 3, 7
		};

		const UINT vertexBufferSize = sizeof(vertices);
		const UINT indexBufferSize = sizeof(indices);

		box = new MeshGeometry();
		D3DCreateBlob(vertexBufferSize, &box->VertexBufferCPU);
		CopyMemory(box->VertexBufferCPU->GetBufferPointer(), vertices, vertexBufferSize);
		D3DCreateBlob(indexBufferSize, &box->IndexBufferCPU);
		CopyMemory(box->IndexBufferCPU->GetBufferPointer(), indices, indexBufferSize);

		box->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(),
			vertices, vertexBufferSize, box->VertexBufferUploader);
		box->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(),
			indices, indexBufferSize, box->IndexBufferUploader);

		box->VertexByteStride = sizeof(Vertex);
		box->VertexBufferByteSize = vertexBufferSize;
		box->IndexFormat = DXGI_FORMAT_R16_UINT;
		box->IndexBufferByteSize = indexBufferSize;

		SubmeshGeometry submesh;
		submesh.IndexCount = _countof(indices);
		submesh.StartIndexLocation = 0;
		submesh.BaseVertexLocation = 0;

		box->DrawArgs["box"] = submesh;
	}

	commandList->Close();
	ID3D12CommandList* cmdsLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);


	{
		device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constantBuffer));

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
		cbvDesc.BufferLocation = constantBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = (sizeof(SceneConstantBuffer) + 255) & ~255;
		device->CreateConstantBufferView(&cbvDesc, cbvHeap->GetCPUDescriptorHandleForHeapStart());

		CD3DX12_RANGE readRange(0, 0);
		constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&cbvDataBegin));
		memcpy(cbvDataBegin, &constantBufferData, sizeof(constantBufferData));
	}


	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	fenceValue = 1;

	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	WaitForPreviousFrame();
	
	
	// D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	// d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x ´ÙÁß »ùÇÃ¸µ
	// d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	// d3dMsaaQualityLevels.NumQualityLevels = 0;
	// 
	// device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	// m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	// m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;


	gnCbvSrvDescriptorIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

//--------------//

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
	WaitForPreviousFrame();

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

	for (int i = 0; i < FrameCount; i++);
	//	if (m_ppd3dSwapChainBackBuffers[i])
	//		m_ppd3dSwapChainBackBuffers[i]->Release();
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	swapChain->GetDesc(&dxgiSwapChainDesc);
	swapChain->ResizeBuffers(FrameCount, CyanFW::Instance()->GetWidth(), CyanFW::Instance()->GetHeight(), dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	frameIndex = swapChain->GetCurrentBackBufferIndex();
}

void RendererManager::WaitForPreviousFrame()
{
	const UINT64 _fence = fenceValue;
	commandQueue->Signal(fence.Get(), _fence);
	++fenceValue;

	if (fence->GetCompletedValue() < _fence)
	{
		fence->SetEventOnCompletion(_fence, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	frameIndex = swapChain->GetCurrentBackBufferIndex();
}
