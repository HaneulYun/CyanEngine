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


	XMMATRIX mWorld = XMLoadFloat4x4(&NS_Matrix4x4::Identity());

	XMMATRIX world = XMLoadFloat4x4(&Camera::main->gameObject->GetMatrix());

	XMVECTOR pos = XMVector3Transform(XMLoadFloat3(&Camera::main->pos), world);
	XMVECTOR lookAt = XMVector3Transform(XMLoadFloat3(&Camera::main->lookAt), world);
	XMVECTOR up{ 0, 1, 0 };

	XMMATRIX view = XMMatrixLookAtLH(pos, lookAt, up);
	XMMATRIX proj = XMLoadFloat4x4(&Camera::main->projection);
	XMMATRIX worldViewProj = mWorld * view * proj;

	PassConstants passConstants;
	XMStoreFloat4x4(&passConstants.ViewProj, XMMatrixTranspose(worldViewProj));
	currFrameResource->PassCB->CopyData(0, passConstants);

	auto currObjectCB = currFrameResource->ObjectCB.get();
	for (auto& e : allRItems)
	{
		if (e->numFramesDirty > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&e->world);

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

			currObjectCB->CopyData(e->objCBIndex, objConstants);

			--e->numFramesDirty;
		}
	}
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

	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetPipelineState(pipelineState.Get());
	ID3D12DescriptorHeap* heaps[]{ cbvHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(heaps), heaps);

	int passCbvIndex = passCbvOffset + currFrameResourceIndex;
	auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvHeap->GetGPUDescriptorHandleForHeapStart());
	passCbvHandle.Offset(passCbvIndex, cbvDescriptorSize);
	commandList->SetGraphicsRootDescriptorTable(1, passCbvHandle);

	UINT cbvIndex = currFrameResourceIndex * (UINT)allRItems.size() + 0;
	auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvHeap->GetGPUDescriptorHandleForHeapStart());
	cbvHandle.Offset(cbvIndex, cbvDescriptorSize);
	commandList->SetGraphicsRootDescriptorTable(0, cbvHandle);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &(geometries["shapeGeo"]->VertexBufferView()));
	commandList->IASetIndexBuffer(&geometries["shapeGeo"]->IndexBufferView());
	commandList->DrawIndexedInstanced(geometries["shapeGeo"]->DrawArgs["box"].IndexCount, 1, 0, 0, 0);

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
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	CD3DX12_DESCRIPTOR_RANGE1 ranges0[1];
	CD3DX12_DESCRIPTOR_RANGE1 ranges1[1];
	CD3DX12_ROOT_PARAMETER1 rootParameters[2];

	ranges0[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	ranges1[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	rootParameters[0].InitAsDescriptorTable(1, &ranges0[0], D3D12_SHADER_VISIBILITY_VERTEX);
	rootParameters[1].InitAsDescriptorTable(1, &ranges1[0], D3D12_SHADER_VISIBILITY_VERTEX);

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

	ComPtr<ID3DBlob> vertexShader = d3dUtil::CompileShader(L"..\\CyanEngine\\shaders\\shaders.hlsl", nullptr, "VSMain", "vs_5_0");
	ComPtr<ID3DBlob> pixelShader = d3dUtil::CompileShader(L"..\\CyanEngine\\shaders\\shaders.hlsl", nullptr, "PSMain", "ps_5_0");

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
	pipelineStateDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	pipelineStateDesc.pRootSignature = rootSignature.Get();
	pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
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


	{
		GeometryGenerator geoGen;
		GeometryGenerator::MeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);;
		GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
		GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
		GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

		UINT boxVertexOffset = 0;
		UINT gridVertexOffset = (UINT)box.Vertices.size();
		UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
		UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();

		UINT boxIndexOffset = 0;
		UINT gridIndexOffset = (UINT)box.Indices32.size();
		UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
		UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

		SubmeshGeometry boxSubmesh;
		boxSubmesh.IndexCount = (UINT)box.Indices32.size();
		boxSubmesh.StartIndexLocation = boxIndexOffset;
		boxSubmesh.BaseVertexLocation = boxVertexOffset;

		SubmeshGeometry gridSubmesh;
		gridSubmesh.IndexCount = (UINT)grid.Indices32.size();
		gridSubmesh.StartIndexLocation = gridIndexOffset;
		gridSubmesh.BaseVertexLocation = gridVertexOffset;

		SubmeshGeometry sphereSubmesh;
		sphereSubmesh.IndexCount = (UINT)sphere.Indices32.size();
		sphereSubmesh.StartIndexLocation = sphereIndexOffset;
		sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

		SubmeshGeometry cylinderSubmesh;
		cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();
		cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
		cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;;

		auto totalVertexCount = box.Vertices.size() + grid.Vertices.size() + sphere.Vertices.size() + cylinder.Vertices.size();

		std::vector<Vertex> vertices(totalVertexCount);

		UINT k = 0;
		for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
		{
			vertices[k].position = box.Vertices[i].Position;
			vertices[k].color = XMFLOAT4(DirectX::Colors::DarkGreen);
		}

		for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
		{
			vertices[k].position = grid.Vertices[i].Position;
			vertices[k].color = XMFLOAT4(DirectX::Colors::ForestGreen);
		}

		for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
		{
			vertices[k].position = sphere.Vertices[i].Position;
			vertices[k].color = XMFLOAT4(DirectX::Colors::Crimson);
		}

		for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
		{
			vertices[k].position = cylinder.Vertices[i].Position;
			vertices[k].color = XMFLOAT4(DirectX::Colors::SteelBlue);
		}

		std::vector<std::uint16_t> indices;
		indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
		indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
		indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
		indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

		const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
		const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

		auto geo = std::make_unique<MeshGeometry>();
		geo->Name = "shapeGeo";

		D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU);
		CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
		D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU);
		CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(),
			vertices.data(), vbByteSize, geo->VertexBufferUploader);
		geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(),
			indices.data(), ibByteSize, geo->IndexBufferUploader);

		geo->VertexByteStride = sizeof(Vertex);
		geo->VertexBufferByteSize = vbByteSize;
		geo->IndexFormat = DXGI_FORMAT_R16_UINT;
		geo->IndexBufferByteSize = ibByteSize;

		geo->DrawArgs["box"] = boxSubmesh;
		geo->DrawArgs["grid"] = gridSubmesh;
		geo->DrawArgs["sphere"] = sphereSubmesh;
		geo->DrawArgs["cylinder"] = cylinderSubmesh;

		geometries[geo->Name] = std::move(geo);
	}

	{
		auto boxRItem = std::make_unique<RenderItem>();
		boxRItem->world = MathHelper::Identity4x4();
		boxRItem->objCBIndex = 0;
		boxRItem->geo = geometries["shapeGeo"].get();
		boxRItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		boxRItem->indexCount = boxRItem->geo->DrawArgs["box"].IndexCount;
		boxRItem->startIndexLocation = boxRItem->geo->DrawArgs["box"].StartIndexLocation;
		boxRItem->baseVertexLocation = boxRItem->geo->DrawArgs["box"].BaseVertexLocation;
		allRItems.push_back(std::move(boxRItem));

		auto gridRItem = std::make_unique<RenderItem>();
		gridRItem->world = MathHelper::Identity4x4();
		gridRItem->objCBIndex = 1;
		gridRItem->geo = geometries["shapeGeo"].get();
		gridRItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		gridRItem->indexCount = gridRItem->geo->DrawArgs["grid"].IndexCount;
		gridRItem->startIndexLocation = gridRItem->geo->DrawArgs["grid"].StartIndexLocation;
		gridRItem->baseVertexLocation = gridRItem->geo->DrawArgs["grid"].BaseVertexLocation;
		allRItems.push_back(std::move(gridRItem));

		UINT objCBIndex = 2;
		for (int i = 0; i < 5; ++i)
		{
			auto leftCylRItem = std::make_unique<RenderItem>();
			auto rightCylRItem = std::make_unique<RenderItem>();
			auto leftSphereRItem = std::make_unique<RenderItem>();
			auto rightSphereRItem = std::make_unique<RenderItem>();

			XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
			XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);
			XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
			XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

			XMStoreFloat4x4(&leftCylRItem->world, leftCylWorld);
			leftCylRItem->objCBIndex = objCBIndex++;
			leftCylRItem->geo = geometries["shapeGeo"].get();
			leftCylRItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			leftCylRItem->indexCount = leftCylRItem->geo->DrawArgs["cylinder"].IndexCount;
			leftCylRItem->startIndexLocation = leftCylRItem->geo->DrawArgs["cylinder"].StartIndexLocation;
			leftCylRItem->baseVertexLocation = leftCylRItem->geo->DrawArgs["cylinder"].BaseVertexLocation;

			XMStoreFloat4x4(&leftCylRItem->world, rightCylWorld);
			rightCylRItem->objCBIndex = objCBIndex++;
			rightCylRItem->geo = geometries["shapeGeo"].get();
			rightCylRItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			rightCylRItem->indexCount = rightCylRItem->geo->DrawArgs["cylinder"].IndexCount;
			rightCylRItem->startIndexLocation = rightCylRItem->geo->DrawArgs["cylinder"].StartIndexLocation;
			rightCylRItem->baseVertexLocation = rightCylRItem->geo->DrawArgs["cylinder"].BaseVertexLocation;

			XMStoreFloat4x4(&leftCylRItem->world, leftSphereWorld);
			leftSphereRItem->objCBIndex = objCBIndex++;
			leftSphereRItem->geo = geometries["shapeGeo"].get();
			leftSphereRItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			leftSphereRItem->indexCount = leftSphereRItem->geo->DrawArgs["sphere"].IndexCount;
			leftSphereRItem->startIndexLocation = leftSphereRItem->geo->DrawArgs["sphere"].StartIndexLocation;
			leftSphereRItem->baseVertexLocation = leftSphereRItem->geo->DrawArgs["sphere"].BaseVertexLocation;

			XMStoreFloat4x4(&leftCylRItem->world, rightSphereWorld);
			rightSphereRItem->objCBIndex = objCBIndex++;
			rightSphereRItem->geo = geometries["shapeGeo"].get();
			rightSphereRItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			rightSphereRItem->indexCount = rightSphereRItem->geo->DrawArgs["sphere"].IndexCount;
			rightSphereRItem->startIndexLocation = rightSphereRItem->geo->DrawArgs["sphere"].StartIndexLocation;
			rightSphereRItem->baseVertexLocation = rightSphereRItem->geo->DrawArgs["sphere"].BaseVertexLocation;

			allRItems.push_back(std::move(leftCylRItem));
			allRItems.push_back(std::move(rightCylRItem));
			allRItems.push_back(std::move(leftSphereRItem));
			allRItems.push_back(std::move(rightSphereRItem));
		}
	}

	for (auto& e : allRItems)
		opaqueRItems.push_back(e.get());


	commandList->Close();
	ID3D12CommandList* cmdsLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	
	for (int i = 0; i < NumFrameResources; ++i)
		frameResources.push_back(std::make_unique<FrameResource>(device.Get(), 1, (UINT)allRItems.size()));


	UINT objCount = (UINT)opaqueRItems.size();
	UINT numDescriptors = (objCount + 1) * NumFrameResources;
	passCbvOffset = objCount * NumFrameResources;

	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&cbvHeap));


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


	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	fenceValue = 1;

	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	WaitForPreviousFrame();
	
	
	// D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	// d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x ���� ���ø�
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
