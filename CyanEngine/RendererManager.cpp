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


	currFrameResourceIndex = (currFrameResourceIndex + 1) % NumFrameResources;
	currFrameResource = frameResources[currFrameResourceIndex].get();

	if (currFrameResource->Fence != 0 && fence->GetCompletedValue() < currFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		fence->SetEventOnCompletion(currFrameResource->Fence, eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	// UpdateObjectCBs
	auto currObjectCB = currFrameResource->ObjectCB.get();
	for (auto& e : allRItems)
	{
		if (e->numFramesDirty > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&e->world);
			XMMATRIX texTransform = XMLoadFloat4x4(&e->texTransform);

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));

			currObjectCB->CopyData(e->objCBIndex, objConstants);

			--e->numFramesDirty;
		}
	}

	// UpdateMaterialCBs
	auto currMaterialCB = currFrameResource->MaterialCB.get();
	for (auto& e : materials)
	{
		Material* mat = e.second.get();
		if (mat->NumFramesDirty > 0)
		{
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

			MaterialConstants matConstants;
			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			matConstants.FresnelR0 = mat->FresnelR0;
			matConstants.Roughness = mat->Roughness;
			XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

			currMaterialCB->CopyData(mat->MatCBIndex, matConstants);

			--mat->NumFramesDirty;
		}
	}
	
	// UpdateMainPassCB
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
	XMVECTOR lightDir = -MathHelper::SphericalToCartesian(1.0f, mSunTheta, mSunPhi);

	XMStoreFloat3(&passConstants.EyePosW, pos);
	XMStoreFloat3(&passConstants.Lights[0].Direction, lightDir);
	passConstants.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	passConstants.Lights[0].Strength = { 1.0f, 1.0f, 0.9f };

	currFrameResource->PassCB->CopyData(0, passConstants);

	// UpdateWaves
	static float time = 0.0f;
	time += Time::deltaTime;

	static float t_base = 0.0f;
	if ((time - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		int i = MathHelper::Rand(4, waves->RowCount() - 5);
		int j = MathHelper::Rand(4, waves->ColumnCount() - 5);

		float r = MathHelper::RandF(0.2f, 0.5f);

		waves->Disturb(i, j, r);
	}

	waves->Update(Time::deltaTime);

	auto currWavesVB = currFrameResource->WavesVB.get();
	for (int i = 0; i < waves->VertexCount(); ++i)
	{
		FrameResource::Vertex v;

		v.Pos = waves->Position(i);
		v.Normal = waves->Normal(i);
		v.TexC.x = 0.5f + v.Pos.x / waves->Width();
		v.TexC.y = 0.5f - v.Pos.z / waves->Depth();

		currWavesVB->CopyData(i, v);
	}

	wavesRItem->geo->VertexBufferGPU = currWavesVB->Resource();
}

void RendererManager::Start()
{
	//static CTerrainShader tshader{ nullptr };
	for (auto& d : instances)
	{
		if (!d.second.first)
		{
			d.second.first = new INSTANCING();
			Shader* shader = new StandardShader();// d.second.first->shader = dynamic_cast<Renderer*>(d.second.second[0]->renderer)->material->shader;

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
	currFrameResource->CmdListAlloc->Reset();
	commandList->Reset(currFrameResource->CmdListAlloc.Get(), pipelineState.Get());

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

	ID3D12DescriptorHeap* heaps[]{ srvHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(heaps), heaps);
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetPipelineState(pipelineState.Get());

	auto passCB = currFrameResource->PassCB->Resource();
	commandList->SetGraphicsRootConstantBufferView(3, passCB->GetGPUVirtualAddress());

	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
	auto objectCB = currFrameResource->ObjectCB->Resource();
	auto matCB = currFrameResource->MaterialCB->Resource();
	for (int i = 0; i < opaqueRItems.size(); ++i)
	{
		auto ri = opaqueRItems[i];

		commandList->IASetVertexBuffers(0, 1, &ri->geo->VertexBufferView());
		commandList->IASetIndexBuffer(&ri->geo->IndexBufferView());
		commandList->IASetPrimitiveTopology(ri->primitiveType);

		CD3DX12_GPU_DESCRIPTOR_HANDLE tex{ srvHeap->GetGPUDescriptorHandleForHeapStart() };
		tex.Offset(ri->mat->DiffuseSrvHeapIndex, srvDescriptorSize);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->objCBIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->mat->MatCBIndex * matCBByteSize;

		commandList->SetGraphicsRootDescriptorTable(0, tex);
		commandList->SetGraphicsRootConstantBufferView(1, objCBAddress);
		commandList->SetGraphicsRootConstantBufferView(2, matCBAddress);

		commandList->DrawIndexedInstanced(ri->indexCount, 1, ri->startIndexLocation, ri->baseVertexLocation, 0);
	}

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
			d.second.first->memory[j].color = dynamic_cast<Renderer*>(gameObject->renderer)->material->DiffuseAlbedo;
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
			d.second.first->memory[j].color = dynamic_cast<Renderer*>(gameObject->renderer)->material->DiffuseAlbedo;
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

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	currFrameResource->Fence = ++fenceValue;
	commandQueue->Signal(fence.Get(), fenceValue);
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
	cbvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


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
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1;

	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER rootParameters[4];
	rootParameters[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[1].InitAsConstantBufferView(0);
	rootParameters[2].InitAsConstantBufferView(1);
	rootParameters[3].InitAsConstantBufferView(2);

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

	ComPtr<ID3DBlob> vertexShader = d3dUtil::CompileShader(L"..\\CyanEngine\\shaders\\shaders.hlsl", nullptr, "VSMain", "vs_5_1");
	ComPtr<ID3DBlob> pixelShader = d3dUtil::CompileShader(L"..\\CyanEngine\\shaders\\shaders.hlsl", nullptr, "PSMain", "ps_5_1");

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
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
	device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState));


	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));


	{
		auto grassTex = std::make_unique<Texture>();
		grassTex->Name = "grassTex";
		grassTex->Filename = L"..\\CyanEngine\\Textures\\grass.dds";
		CreateDDSTextureFromFile12(device.Get(), commandList.Get(), grassTex->Filename.c_str(), grassTex->Resource, grassTex->UploadHeap);

		auto waterTex = std::make_unique<Texture>();
		waterTex->Name = "waterTex";
		waterTex->Filename = L"..\\CyanEngine\\Textures\\water1.dds";
		CreateDDSTextureFromFile12(device.Get(), commandList.Get(), waterTex->Filename.c_str(), waterTex->Resource, waterTex->UploadHeap);

		auto fenceTex = std::make_unique<Texture>();
		fenceTex->Name = "fenceTex";
		fenceTex->Filename = L"..\\CyanEngine\\Textures\\WoodCrate01.dds";
		CreateDDSTextureFromFile12(device.Get(), commandList.Get(), fenceTex->Filename.c_str(), fenceTex->Resource, fenceTex->UploadHeap);

		textures[grassTex->Name] = std::move(grassTex);
		textures[waterTex->Name] = std::move(waterTex);
		textures[fenceTex->Name] = std::move(fenceTex);
	}
	{
		auto grass = std::make_unique<Material>();
		grass->Name = "grass";
		grass->MatCBIndex = 0;
		grass->DiffuseSrvHeapIndex = 0;
		grass->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		grass->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
		grass->Roughness = 0.125f;

		auto water = std::make_unique<Material>();
		water->Name = "water";
		water->MatCBIndex = 1;
		water->DiffuseSrvHeapIndex = 1;
		water->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		water->FresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
		water->Roughness = 0.f;

		materials["grass"] = std::move(grass);
		materials["water"] = std::move(water);
	}
	{
		GeometryGenerator geoGen;
		GeometryGenerator::MeshData grid = geoGen.CreateGrid(160.0f, 160.0f, 50, 50);

		std::vector<FrameResource::Vertex> vertices(grid.Vertices.size());
		for (size_t i = 0; i < grid.Vertices.size(); ++i)
		{
			auto& p = grid.Vertices[i].Position;
			vertices[i].Pos = p;
			vertices[i].Pos.y = 0.3f * (p.z * sinf(0.1f * p.x) + p.x * cosf(0.1f * p.z));

			XMFLOAT3 n(
				-0.03f * p.z * cosf(0.1f * p.x) - 0.3f * cosf(0.1f * p.z),
				1.0f,
				-0.3f * sinf(0.1f * p.x) + 0.03f * p.x * sinf(0.1f * p.z));
			XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
			XMStoreFloat3(&n, unitNormal);

			vertices[i].Normal = n;
			vertices[i].TexC = grid.Vertices[i].TexC;
		}
		std::vector<std::uint16_t> indices = grid.GetIndices16();

		const UINT vbByteSize = (UINT)vertices.size() * sizeof(FrameResource::Vertex);
		const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

		auto geo = std::make_unique<MeshGeometry>();
		geo->Name = "landGeo";

		D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU);
		CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
		D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU);
		CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);
		geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

		geo->VertexByteStride = sizeof(FrameResource::Vertex);
		geo->VertexBufferByteSize = vbByteSize;
		geo->IndexFormat = DXGI_FORMAT_R16_UINT;
		geo->IndexBufferByteSize = ibByteSize;

		SubmeshGeometry submesh;
		submesh.IndexCount = (UINT)grid.Indices32.size();
		submesh.StartIndexLocation = 0;
		submesh.BaseVertexLocation = 0;

		geo->DrawArgs["grid"] = submesh;

		geometries[geo->Name] = std::move(geo);
	}
	{
		waves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);
		std::vector<std::uint16_t> indices(3 * waves->TriangleCount());

		int m = waves->RowCount();
		int n = waves->ColumnCount();
		int k = 0;
		for (int i = 0; i < m - 1; ++i)
		{
			for (int j = 0; j < n - 1; ++j)
			{
				indices[k] = i * n + j;
				indices[k + 1] = i * n + j + 1;
				indices[k + 2] = (i + 1) * n + j;

				indices[k + 3] = (i + 1) * n + j;
				indices[k + 4] = i * n + j + 1;
				indices[k + 5] = (i + 1) * n + j + 1;

				k += 6;
			}
		}
		UINT vbByteSize = waves->VertexCount() * sizeof(FrameResource::Vertex);
		UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

		auto geo = std::make_unique<MeshGeometry>();
		geo->Name = "waterGeo";

		geo->VertexBufferCPU = nullptr;
		geo->VertexBufferGPU = nullptr;

		D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU);
		CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(),
			indices.data(), ibByteSize, geo->IndexBufferUploader);

		geo->VertexByteStride = sizeof(FrameResource::Vertex);
		geo->VertexBufferByteSize = vbByteSize;
		geo->IndexFormat = DXGI_FORMAT_R16_UINT;
		geo->IndexBufferByteSize = ibByteSize;

		SubmeshGeometry submesh;
		submesh.IndexCount = (UINT)indices.size();
		submesh.StartIndexLocation = 0;
		submesh.BaseVertexLocation = 0;

		geo->DrawArgs["grid"] = submesh;

		geometries[geo->Name] = std::move(geo);
	}
	{
		auto wavesRItem = std::make_unique<RenderItem>();
		wavesRItem->world = MathHelper::Identity4x4();
		XMStoreFloat4x4(&wavesRItem->texTransform, XMMatrixScaling(5.0f, 5.0f, 1.0f));
		wavesRItem->objCBIndex = 0;
		wavesRItem->mat = materials["water"].get();
		wavesRItem->geo = geometries["waterGeo"].get();
		wavesRItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		wavesRItem->indexCount = wavesRItem->geo->DrawArgs["grid"].IndexCount;
		wavesRItem->startIndexLocation = wavesRItem->geo->DrawArgs["grid"].StartIndexLocation;
		wavesRItem->baseVertexLocation = wavesRItem->geo->DrawArgs["grid"].BaseVertexLocation;
		this->wavesRItem = wavesRItem.get();
		allRItems.push_back(std::move(wavesRItem));

		auto gridRItem = std::make_unique<RenderItem>();
		gridRItem->world = MathHelper::Identity4x4();
		XMStoreFloat4x4(&gridRItem->texTransform, XMMatrixScaling(5.0f, 5.0f, 1.0f));
		gridRItem->objCBIndex = 1;
		gridRItem->mat = materials["grass"].get();
		gridRItem->geo = geometries["landGeo"].get();
		gridRItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		gridRItem->indexCount = gridRItem->geo->DrawArgs["grid"].IndexCount;
		gridRItem->startIndexLocation = gridRItem->geo->DrawArgs["grid"].StartIndexLocation;
		gridRItem->baseVertexLocation = gridRItem->geo->DrawArgs["grid"].BaseVertexLocation;
		allRItems.push_back(std::move(gridRItem));
	}

	for (auto& e : allRItems)
		opaqueRItems.push_back(e.get());


	commandList->Close();
	ID3D12CommandList* cmdsLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	
	for (int i = 0; i < NumFrameResources; ++i)
		frameResources.push_back(std::make_unique<FrameResource>(device.Get(), 1, (UINT)allRItems.size(), (UINT)materials.size(), waves->VertexCount()));


	UINT objCount = (UINT)opaqueRItems.size();
	UINT numDescriptors = (objCount + 1) * NumFrameResources;
	passCbvOffset = objCount * NumFrameResources;

	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&cbvHeap));

	descriptorHeapDesc.NumDescriptors = 3;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&srvHeap));


	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	for(int i = 0; i < NumFrameResources; ++i)
	{
		auto objectCB = frameResources[i]->ObjectCB->Resource();
		for (UINT j = 0; j < objCount; ++j)
		{
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->GetGPUVirtualAddress();
			cbAddress += j * objCBByteSize;

			int heapIndex = i * objCount + j;
			auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHeap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset(heapIndex, cbvDescriptorSize);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
			cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes = objCBByteSize;
			device->CreateConstantBufferView(&cbvDesc, handle);
		}
	}

	UINT passCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants));
	for(int i = 0; i < NumFrameResources; ++i)
	{
		auto passCB = frameResources[i]->PassCB->Resource();
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

		int heapIndex = passCbvOffset + i;
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(heapIndex, cbvDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = passCBByteSize;
		device->CreateConstantBufferView(&cbvDesc, handle);
	}

	{
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(srvHeap->GetCPUDescriptorHandleForHeapStart());
		auto grassTex = textures["grassTex"]->Resource;
		auto waterTex = textures["waterTex"]->Resource;
		auto fenceTex = textures["fenceTex"]->Resource;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = grassTex->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;
		device->CreateShaderResourceView(grassTex.Get(), &srvDesc, handle);

		handle.Offset(1, srvDescriptorSize);
		srvDesc.Format = waterTex->GetDesc().Format;
		device->CreateShaderResourceView(waterTex.Get(), &srvDesc, handle);

		handle.Offset(1, srvDescriptorSize);
		srvDesc.Format = fenceTex->GetDesc().Format;
		device->CreateShaderResourceView(fenceTex.Get(), &srvDesc, handle);
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
