#include "pch.h"
#include "Graphics.h"

extern UINT gnCbvSrvDescriptorIncrementSize;

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
}

void Graphics::Initialize()
{
	InitDirect3D();
	LoadAssets();
	CreateDepthStencilView();
}

void Graphics::Start()
{
}

void Graphics::Update(std::vector<std::unique_ptr<FrameResource>>& frameResources)
{
	currFrameResourceIndex = (currFrameResourceIndex + 1) % NumFrameResources;
	currFrameResource = frameResources[currFrameResourceIndex].get();

	if (currFrameResource->Fence != 0 && fence->GetCompletedValue() < currFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		fence->SetEventOnCompletion(currFrameResource->Fence, eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	SkinnedModelInstance* skinnedModelInst{ nullptr };
	// UpdateObjectCBs
	auto currObjectCB = currFrameResource->ObjectCB.get();
	for (auto& e : Scene::scene->allRItems)
	{
		if (e->NumFramesDirty > 0)
		{
			XMMATRIX world;
			auto transform = e->gameObject->GetComponent<Transform>();
			if (transform)
				world = XMLoadFloat4x4(&transform->localToWorldMatrix);

			XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
			objConstants.MaterialIndex = e->Mat->MatCBIndex;

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			--e->NumFramesDirty;
		}

		if (e->SkinnedModelInst)
		{
			skinnedModelInst = e->SkinnedModelInst;
		}
	}
	if (skinnedModelInst)
	{
		// UpdateSkinnedCBs
		auto currSkinnedCB = currFrameResource->SkinnedCB.get();

		// We only have one skinned model being animated.
		skinnedModelInst->UpdateSkinnedAnimation(Time::deltaTime);

		SkinnedConstants skinnedConstants;
		std::copy(
			std::begin(skinnedModelInst->FinalTransforms),
			std::end(skinnedModelInst->FinalTransforms),
			&skinnedConstants.BoneTransforms[0]);

		currSkinnedCB->CopyData(0, skinnedConstants);
	}

	// UpdateMaterialBuffer
	auto currMaterialBuffer = currFrameResource->MaterialBuffer.get();
	for (auto& e : Scene::scene->materials)
	{
		Material* mat = e.second.get();
		if (mat->NumFramesDirty > 0)
		{
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

			MaterialData matData;
			matData.DiffuseAlbedo = mat->DiffuseAlbedo;
			matData.FresnelR0 = mat->FresnelR0;
			matData.Roughness = mat->Roughness;
			XMStoreFloat4x4(&matData.MatTransform, XMMatrixTranspose(matTransform));
			matData.DiffuseMapIndex = mat->DiffuseSrvHeapIndex;

			currMaterialBuffer->CopyData(mat->MatCBIndex, matData);

			--mat->NumFramesDirty;
		}
	}

	// UpdateMainPassCB
	{
		PassConstants passConstants;

		XMMATRIX world = XMLoadFloat4x4(&Camera::main->gameObject->GetMatrix());
		XMVECTOR pos = XMVector3Transform(XMLoadFloat3(&Camera::main->pos), world);
		XMVECTOR lookAt = XMVector3Transform(XMLoadFloat3(&Camera::main->lookAt), world);
		XMVECTOR up{ 0, 1, 0 };

		XMMATRIX view = XMMatrixLookAtLH(pos, lookAt, up);
		XMMATRIX proj = XMLoadFloat4x4(&Camera::main->projection);
		XMMATRIX worldViewProj = view * proj;

		XMStoreFloat4x4(&passConstants.ViewProj, XMMatrixTranspose(worldViewProj));

		float mSunTheta = 1.25f * XM_PI;
		float mSunPhi = XM_PIDIV4;

		XMStoreFloat3(&passConstants.EyePosW, pos);
		passConstants.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
		passConstants.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
		passConstants.Lights[0].Strength = { 0.8f, 0.8f, 0.8f };
		passConstants.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
		passConstants.Lights[1].Strength = { 0.4f, 0.4f, 0.4f };
		passConstants.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
		passConstants.Lights[2].Strength = { 0.2f, 0.2f, 0.2f };

		currFrameResource->PassCB->CopyData(0, passConstants);
	}
}

void Graphics::PreRender()
{
	currFrameResource->CmdListAlloc->Reset();
	commandList->Reset(currFrameResource->CmdListAlloc.Get(), nullptr);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle{ dsvHeap->GetCPUDescriptorHandleForHeapStart() };
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	const float clearColor[] { 0.1921569, 0.3019608, 0.4745098, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	D3D12_CLEAR_FLAGS clearFlags{ D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL };
	commandList->ClearDepthStencilView(dsvHandle, clearFlags, 1.0f, 0, 0, nullptr);
}

void Graphics::Render()
{
	PreRender();

	Camera::main->SetViewportsAndScissorRects(commandList.Get());

	ID3D12DescriptorHeap* heaps[]{ srvHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(heaps), heaps);
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetPipelineState(pipelineStates["opaque"].Get());

	auto passCB = currFrameResource->PassCB->Resource();
	auto matBuffer = currFrameResource->MaterialBuffer->Resource();

	commandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(4, srvHeap->GetGPUDescriptorHandleForHeapStart());

	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT skinnedCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(SkinnedConstants));
	auto objectCB = currFrameResource->ObjectCB->Resource();
	auto skinnedCB = currFrameResource->SkinnedCB->Resource();

	int k = 0;
	for (auto& renderItems : Scene::scene->renderItemLayer)
	{
		if (k++ == (int)RenderLayer::SkinnedOpaque)
			commandList->SetPipelineState(pipelineStates["skinnedOpaque"].Get());
		else
			commandList->SetPipelineState(pipelineStates["opaque"].Get());

		for (int i = 0; i < renderItems.size(); ++i)
		{
			auto ri = renderItems[i];

			commandList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
			commandList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
			commandList->IASetPrimitiveTopology(ri->PrimitiveType);

			D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;
			commandList->SetGraphicsRootConstantBufferView(0, objCBAddress);

			if (ri->SkinnedModelInst != nullptr)
			{
				D3D12_GPU_VIRTUAL_ADDRESS skinnedCBAddress = skinnedCB->GetGPUVirtualAddress() + ri->SkinnedCBIndex * skinnedCBByteSize;
				commandList->SetGraphicsRootConstantBufferView(1, skinnedCBAddress);
			}
			else
			{
				commandList->SetGraphicsRootConstantBufferView(1, 0);
			}

			commandList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
		}
	}

	PostRender();
}

void Graphics::PostRender()
{
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	commandList->Close();

	ID3D12CommandList* ppd3dCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists), ppd3dCommandLists);

	swapChain->Present(0, 0);

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	currFrameResource->Fence = ++fenceValue;
	commandQueue->Signal(fence.Get(), fenceValue);
}

void Graphics::Destroy()
{
	WaitForPreviousFrame();

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();

	swapChain->SetFullscreenState(FALSE, NULL);
}

void Graphics::InitDirect3D()
{
	UINT dxgiFactoryFlags{ 0 };

#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debug{ nullptr };
	D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	//debug->EnableDebugLayer();

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

	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	cbvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	
	device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels));
	m_nMsaa4xQualityLevels = msQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;


	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
	commandList->Close();


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

	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&dsvHeap));


	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT i = 0; i < FrameCount; ++i)
	{
		swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
		device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, rtvDescriptorSize);
	}
}

void Graphics::LoadAssets()
{
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1;

	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 9, 0, 0, 0);

	CD3DX12_ROOT_PARAMETER rootParameters[5];
	rootParameters[0].InitAsConstantBufferView(0);
	rootParameters[1].InitAsConstantBufferView(1);
	rootParameters[2].InitAsConstantBufferView(2);
	rootParameters[3].InitAsShaderResourceView(0, 1);
	rootParameters[4].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_STATIC_SAMPLER_DESC staticSamplers[]
	{
		{ 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP },		// pointWrap
		{ 1, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP },	// pointClamp
		{ 2, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP },		// linearWrap
		{ 3, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP },	// linearClamp
		{ 4, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0.0f, 8 },	// anisotropicWrap
		{ 5, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 0.0f, 8 }	// anisotropicClamp
	};

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags{
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
	};

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(_countof(rootParameters), rootParameters, _countof(staticSamplers), staticSamplers, rootSignatureFlags);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	D3D12SerializeRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error);
	device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

	const D3D_SHADER_MACRO skinnedDefines[] =
	{
		"SKINNED", "1",
		NULL, NULL
	};
	ComPtr<ID3DBlob> vertexShader_skinned = d3dUtil::CompileShader(L"..\\CyanEngine\\shaders\\shaders.hlsl", skinnedDefines, "VSMain", "vs_5_1");
	ComPtr<ID3DBlob> vertexShader = d3dUtil::CompileShader(L"..\\CyanEngine\\shaders\\shaders.hlsl", nullptr, "VSMain", "vs_5_1");
	ComPtr<ID3DBlob> pixelShader = d3dUtil::CompileShader(L"..\\CyanEngine\\shaders\\shaders.hlsl", nullptr, "PSMain", "ps_5_1");

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_skinned[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BONEINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 56, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
	pipelineStateDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	pipelineStateDesc.pRootSignature = rootSignature.Get();
	pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.NumRenderTargets = 1;
	pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pipelineStateDesc.SampleDesc.Count = 1;
	device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineStates["opaque"]));

	pipelineStateDesc.InputLayout = { inputElementDescs_skinned, _countof(inputElementDescs_skinned) };
	pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader_skinned.Get());
	device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineStates["skinnedOpaque"]));

	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.NumDescriptors = 9;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&srvHeap));

	WaitForPreviousFrame();

	gnCbvSrvDescriptorIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

//--------------//

inline void Graphics::CreateDepthStencilView()
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

void Graphics::ChangeSwapChainState()
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

void Graphics::WaitForPreviousFrame()
{
	const UINT64 _fence = fenceValue;
	commandQueue->Signal(fence.Get(), _fence);
	++fenceValue;

	if (fence->GetCompletedValue() < _fence)
	{
		HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		fence->SetEventOnCompletion(_fence, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
		CloseHandle(fenceEvent);
	}

	frameIndex = swapChain->GetCurrentBackBufferIndex();
}
