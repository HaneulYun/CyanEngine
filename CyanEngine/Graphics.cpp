#include "pch.h"
#include "Graphics.h"

void Graphics::Initialize()
{
	sceneBounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	sceneBounds.Radius = sqrtf(20.0f * 20.0f + 20.0f * 20.0f);
	InitDirect3D();
	InitDirect2D();
	LoadAssets();
	CreateDepthStencilView();
}

void Graphics::PreRender()
{
	FrameResource* currFrameResource = Scene::scene->frameResourceManager.currFrameResource;
	int currFrameResourceIndex = Scene::scene->frameResourceManager.currFrameResourceIndex;

	currFrameResource->CmdListAlloc->Reset();
	commandList->Reset(currFrameResource->CmdListAlloc.Get(), nullptr);

	ID3D12DescriptorHeap* heaps[]{ srvHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(heaps), heaps);
	commandList->SetGraphicsRootSignature(rootSignature.Get());

	auto matBuffer = AssetManager::Instance()->assetResource[Scene::scene->frameResourceManager.currFrameResourceIndex]->MaterialBuffer->Resource();
	commandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(4, srvHeap->GetGPUDescriptorHandleForHeapStart());

	RenderShadowMap();
}

void Graphics::RenderShadowMap()
{
	FrameResource* currFrameResource = Scene::scene->frameResourceManager.currFrameResource;
	int currFrameResourceIndex = Scene::scene->frameResourceManager.currFrameResourceIndex;

	commandList->RSSetViewports(1, &shadowMap->Viewport());
	commandList->RSSetScissorRects(1, &shadowMap->ScissorRect());

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(shadowMap->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	commandList->ClearDepthStencilView(shadowMap->Dsv(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	commandList->OMSetRenderTargets(0, nullptr, false, &shadowMap->Dsv());

	UINT passCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants));
	auto passCB = currFrameResource->PassCB->Resource();
	D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = passCB->GetGPUVirtualAddress() + 1 * passCBByteSize;
	commandList->SetGraphicsRootConstantBufferView(2, passCBAddress);

	commandList->SetPipelineState(pipelineStates["shadow_opaque"].Get());

	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(InstanceData));

	for (int layerIndex = 0; layerIndex < (int)RenderLayer::Count; ++layerIndex)
		RenderObjects(layerIndex, true);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(shadowMap->Resource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void Graphics::Render()
{
	FrameResource* currFrameResource = Scene::scene->frameResourceManager.currFrameResource;
	int currFrameResourceIndex = Scene::scene->frameResourceManager.currFrameResourceIndex;
	if (currFrameResource->Fence != 0 && fence->GetCompletedValue() < currFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		fence->SetEventOnCompletion(currFrameResource->Fence, eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	PreRender();

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle{ dsvHeap->GetCPUDescriptorHandleForHeapStart() };
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	const float clearColor[]{ 0.1921569, 0.3019608, 0.4745098, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	D3D12_CLEAR_FLAGS clearFlags{ D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL };
	commandList->ClearDepthStencilView(dsvHandle, clearFlags, 1.0f, 0, 0, nullptr);


	D3D12_VIEWPORT viewport{
		Camera::main->viewport.x,  Camera::main->viewport.y,
		Camera::main->viewport.w * CyanFW::Instance()->GetWidth(),
		Camera::main->viewport.h * CyanFW::Instance()->GetHeight(),
		0.0f, 1.0f
	};
	D3D12_RECT scissorRect{ 0.0f, 0.0f, CyanFW::Instance()->GetWidth(), CyanFW::Instance()->GetHeight() };
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);

	auto passCB = currFrameResource->PassCB->Resource();
	
	commandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

	for(int layerIndex = 0; layerIndex < (int)RenderLayer::Count; ++layerIndex)
		RenderObjects(layerIndex);
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


	PostRender();
}

void Graphics::RenderObjects(int layerIndex, bool isShadowMap)
{
	if (isShadowMap)
	{
		if (layerIndex == (int)RenderLayer::Particle ||
			layerIndex == (int)RenderLayer::Sky ||
			layerIndex == (int)RenderLayer::UI ||
			layerIndex == (int)RenderLayer::BuildPreview ||
			layerIndex == (int)RenderLayer::Grass)
			return;
		else if (layerIndex == (int)RenderLayer::SkinnedOpaque)
			commandList->SetPipelineState(pipelineStates["shadow_skinnedOpaque"].Get());
		else
			commandList->SetPipelineState(pipelineStates["shadow_opaque"].Get());
	}
	else
	{
		if (layerIndex == (int)RenderLayer::SkinnedOpaque)
			commandList->SetPipelineState(pipelineStates["skinnedOpaque"].Get());
		else if (layerIndex == (int)RenderLayer::Sky)
			commandList->SetPipelineState(pipelineStates["sky"].Get());
		else if (layerIndex == (int)RenderLayer::Grass)
			commandList->SetPipelineState(pipelineStates["grass"].Get());
		else if (layerIndex == (int)RenderLayer::BuildPreview)
			commandList->SetPipelineState(pipelineStates["buildPreview"].Get());
		else if (layerIndex == (int)RenderLayer::UI)
		{
			commandList->OMSetStencilRef(1);
			commandList->SetPipelineState(pipelineStates["ui"].Get());
		}
		else
			commandList->SetPipelineState(pipelineStates["opaque"].Get());
	}

	for (auto& renderSets : Scene::scene->objectRenderManager.renderObjectsLayer[layerIndex])
	{
		auto& mesh = renderSets.first;
		auto& objects = renderSets.second.gameObjects;
		if (!objects.size())
			continue;

		auto objectsResource = renderSets.second.GetResources();
		auto instanceBuffer = objectsResource->InstanceBuffer.get();
		auto skinnedBuffer = objectsResource->SkinnedBuffer.get();
		auto matIndexBuffer = objectsResource->MatIndexBuffer.get();

		if (layerIndex == (int)RenderLayer::BuildPreview)
		{
			Vector4 v = objects.back()->GetComponent<Constant>()->v4;
			commandList->SetGraphicsRoot32BitConstants(0, 4, &v.xmf4, 0);
		}
		commandList->SetGraphicsRootShaderResourceView(5, instanceBuffer->Resource()->GetGPUVirtualAddress());
		commandList->SetGraphicsRootShaderResourceView(6, skinnedBuffer->Resource()->GetGPUVirtualAddress());

		commandList->IASetPrimitiveTopology(mesh->PrimitiveType);

		int i = 0;
		if (layerIndex != (int)RenderLayer::Particle)
		{
			commandList->IASetVertexBuffers(0, 1, &mesh->VertexBufferView());
			if (mesh->IndexBufferByteSize)
				commandList->IASetIndexBuffer(&mesh->IndexBufferView());
			for (auto& submesh : mesh->DrawArgs)
			{
				commandList->SetGraphicsRootShaderResourceView(7, matIndexBuffer->Resource()->GetGPUVirtualAddress() + sizeof(MatIndexData) * i++);
				if (mesh->IndexBufferByteSize)
					commandList->DrawIndexedInstanced(submesh.second.IndexCount, objects.size(), submesh.second.StartIndexLocation, submesh.second.BaseVertexLocation, 0);
				else
					commandList->DrawInstanced(submesh.second.IndexCount, objects.size(), submesh.second.StartIndexLocation, 0);
			}
		}
		else
		{
			commandList->SetGraphicsRootShaderResourceView(7, matIndexBuffer->Resource()->GetGPUVirtualAddress());
			auto temp = mesh;
			auto mesh = (ParticleBundle*)temp;
			commandList->SetPipelineState(pipelineStates["particleMaker"].Get());
			
			char* data = new char[sizeof(UINT64)];
			memset(data, 0, sizeof(UINT64));
			D3D12_SUBRESOURCE_DATA subResourceData = { data, sizeof(UINT64), sizeof(UINT64) };

			ParticleResource* res = mesh->particleResource[Scene::scene->frameResourceManager.currFrameResourceIndex % 3];
			ParticleResource* resource = mesh->particleResource[(Scene::scene->frameResourceManager.currFrameResourceIndex + 1) % 3];
			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource->VertexParticleBufferGPU.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
			UpdateSubresources<1>(commandList.Get(), resource->VertexParticleBufferGPU.Get(), resource->VertexParticleBufferUploader.Get(), 0, 0, 1, &subResourceData);
			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource->VertexParticleBufferGPU.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
			delete data;
			
			commandList->SOSetTargets(0, 1, &mesh->StreamOutputBufferView());
			commandList->IASetVertexBuffers(0, 1, &mesh->VertexBufferView());
			commandList->DrawInstanced(res->particleCount, objects.size(), 0, 0);
			commandList->SOSetTargets(0, 1, &mesh->StreamOutputBufferView());
			
			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource->VertexParticleBufferGPU.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_SOURCE));
			commandList->CopyResource(mesh->VertexParticleBufferReadback.Get(), resource->VertexParticleBufferGPU.Get());
			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource->VertexParticleBufferGPU.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON));
			
			commandList->SetPipelineState(pipelineStates["particle"].Get());
			commandList->DrawInstanced(res->particleCount, objects.size(), 0, 0);
		}
	}
}

void Graphics::RenderUI()
{
	UINT width = CyanFW::Instance()->GetWidth();
	UINT height = CyanFW::Instance()->GetHeight();

	device11On12->AcquireWrappedResources(wrappedBackBuffers[frameIndex].GetAddressOf(), 1);
	deviceContext->SetTarget(renderTargets2d[frameIndex].Get());
	deviceContext->BeginDraw();
	deviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

	for (GameObject* gameObject : Scene::scene->textObjects)
	{
		Text* textComponent = gameObject->GetComponent<Text>();
		//if (textComponent == nullptr) continue;

		if (textComponent->brushIndex == -1)
		{
			for (int i = 0; i < textBrushes.size(); ++i)
			{
				if (!std::memcmp(&textComponent->color, &textBrushes[i]->GetColor(), sizeof(D2D1_COLOR_F)))
				{
					textComponent->brushIndex = i;
					break;
				}
			}
			if (textComponent->brushIndex == -1)
			{
				ComPtr<ID2D1SolidColorBrush> textBrush;
				D2D_COLOR_F color{ textComponent->color.r, textComponent->color.g,textComponent->color.b,textComponent->color.a };
				deviceContext->CreateSolidColorBrush(color, &textBrush);
				textComponent->brushIndex = textBrushes.size();
				textBrushes.push_back(textBrush);
			}
		}

		if (textComponent->formatIndex == -1)
		{
			for (int i = 0; i < textFormats.size(); ++i)
			{
				WCHAR name[15];
				textFormats[i]->GetFontFamilyName(name, textFormats[i]->GetFontFamilyNameLength());

				if (name == textComponent->font.c_str() &&
					textFormats[i]->GetFontSize() == textComponent->fontSize &&
					textFormats[i]->GetParagraphAlignment() == textComponent->paragraphAlignment &&
					textFormats[i]->GetTextAlignment() == textComponent->textAlignment &&
					textFormats[i]->GetFontWeight() == textComponent->fontWeight &&
					textFormats[i]->GetFontStyle() == textComponent->style)
				{
					textComponent->formatIndex = i;
					break;
				}
			}
			if (textComponent->formatIndex == -1)
			{
				ComPtr<IDWriteTextFormat> textFormat;
				writeFactory->CreateTextFormat(
					textComponent->font.c_str(),
					NULL,
					textComponent->fontWeight,
					textComponent->style,
					DWRITE_FONT_STRETCH_NORMAL,
					textComponent->fontSize,
					L"ko-KR",
					&textFormat
				);
				textFormat->SetTextAlignment(textComponent->textAlignment);
				textFormat->SetParagraphAlignment(textComponent->paragraphAlignment);
				textComponent->formatIndex = textFormats.size();
				textFormats.push_back(textFormat);
			}
		}

		D2D_RECT_F rt;

		RectTransform* rect = gameObject->GetComponent<RectTransform>();
		Matrix4x4 mat = RectTransform::Transform(gameObject->GetMatrix());

		Vector3 leftTop{ mat._41, mat._22 + mat._42, 0 };
		Vector3 rightBottom{ mat._11 + mat._41, mat._42, 0 };

		rt.left = (leftTop.x / 2.0f + 0.5f) * width;
		rt.top = (leftTop.y / -2.0f + 0.5f) * height;
		rt.right = (rightBottom.x / 2.0f + 0.5f) * width;
		rt.bottom = (rightBottom.y / -2.0f + 0.5f) * height;

		if(textComponent->gameObject->active)
			deviceContext->DrawText(textComponent->text.c_str(), textComponent->text.length(), textFormats[textComponent->formatIndex].Get(), &rt, textBrushes[textComponent->brushIndex].Get());
	}

	deviceContext->EndDraw();
	device11On12->ReleaseWrappedResources(wrappedBackBuffers[frameIndex].GetAddressOf(), 1);

	d11DeviceContext->Flush();
}

void Graphics::PostRender()
{
	FrameResource* currFrameResource = Scene::scene->frameResourceManager.currFrameResource;

	commandList->Close();

	ID3D12CommandList* ppd3dCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists), ppd3dCommandLists);

	RenderUI();
	swapChain->Present(0, 0);

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	currFrameResource->Fence = ++fenceValue;
	commandQueue->Signal(fence.Get(), fenceValue);

	//WaitForPreviousFrame();

	for (auto& renderSets : Scene::scene->objectRenderManager.renderObjectsLayer[(int)RenderLayer::Particle])
	{
		auto mesh = (ParticleBundle*)renderSets.first;
		
		UINT8* p;
		mesh->VertexParticleBufferReadback->Map(0, NULL, reinterpret_cast<void**>(&p));
		
		UINT64 size = *reinterpret_cast<UINT64*>(p);
		FrameResource::ParticleSpriteVertex* t = reinterpret_cast<FrameResource::ParticleSpriteVertex*>(p+8);
		
		ParticleResource* resource = mesh->particleResource[(Scene::scene->frameResourceManager.currFrameResourceIndex + 1) % 3];
		resource->particleCount = size / sizeof(FrameResource::ParticleSpriteVertex);
		
		mesh->VertexParticleBufferReadback->Unmap(0, nullptr);
	}
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


	descriptorHeapDesc.NumDescriptors = 2;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&dsvHeap));

	shadowMap = std::make_unique<ShadowMap>(device.Get(), 2048, 2048);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT i = 0; i < FrameCount; ++i)
	{
		swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
		device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);

		rtvHandle.Offset(1, rtvDescriptorSize);
	}

}

void Graphics::InitDirect2D()
{
	// Create an 11 device wrapped around the 12 device and share
	// 12's command queue.

	UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};

	ComPtr<ID3D11Device> d3d11Device;
	D3D11On12CreateDevice(device.Get(), d3d11DeviceFlags, nullptr, 0, reinterpret_cast<IUnknown**>(commandQueue.GetAddressOf()),
		1, 0, &d3d11Device, &d11DeviceContext, nullptr);
	d3d11Device.As(&device11On12);

	// Create D2D/DWrite components.
	ComPtr<ID2D1Factory3> d2dFactory{ nullptr };
	{
		D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, &d2dFactory);
		ComPtr<IDXGIDevice> dxgiDevice;
		device11On12.As(&dxgiDevice);
		d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dDevice);
		d2dDevice->CreateDeviceContext(deviceOptions, &deviceContext);
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &writeFactory);
	}

	// Query the desktop's dpi settings, which will be used to create
	// D2D's render targets.
	float dpiX;
	float dpiY;
	d2dFactory->GetDesktopDpi(&dpiX, &dpiY);
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpiX, dpiY);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT i = 0; i < FrameCount; ++i)
	{
		D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
		device11On12->CreateWrappedResource(renderTargets[i].Get(), &d3d11Flags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(&wrappedBackBuffers[i]));

		ComPtr<IDXGISurface> surface;
		wrappedBackBuffers[i].As(&surface);
		deviceContext->CreateBitmapFromDxgiSurface(surface.Get(), &bitmapProperties, &renderTargets2d[i]);
	}
}

void Graphics::LoadAssets()
{
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1;

	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 7, 0, 0, 0);

	CD3DX12_ROOT_PARAMETER rootParameters[8];
	//rootParameters[0].InitAsConstantBufferView(0);
	rootParameters[0].InitAsConstants(4, 0);
	rootParameters[1].InitAsConstantBufferView(1);
	rootParameters[2].InitAsConstantBufferView(2);
	rootParameters[3].InitAsShaderResourceView(1, 1);
	rootParameters[4].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[5].InitAsShaderResourceView(0, 1);
	rootParameters[6].InitAsShaderResourceView(2, 1);
	rootParameters[7].InitAsShaderResourceView(3, 1);

	CD3DX12_STATIC_SAMPLER_DESC staticSamplers[]
	{
		{ 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP },		// pointWrap
		{ 1, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP },	// pointClamp
		{ 2, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP },		// linearWrap
		{ 3, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP },	// linearClamp
		{ 4, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0.0f, 8 },	// anisotropicWrap
		{ 5, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 0.0f, 8 },	// anisotropicClamp
		{ 6, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER,D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		0.0f, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL,D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK } // shaderRegister
	};

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags{
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT

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
	const D3D_SHADER_MACRO alphaTestDefines[] =
	{
		"ALPHA_TEST", "1",
		NULL, NULL
	};

	ComPtr<ID3DBlob> vertexShader_skinnedShadow = d3dUtil::CompileShader(L"shaders\\Shadow.hlsl", skinnedDefines, "VS_Shadow", "vs_5_1");
	ComPtr<ID3DBlob> vertexShader_shadow = d3dUtil::CompileShader(L"shaders\\Shadow.hlsl", nullptr, "VS_Shadow", "vs_5_1");
	ComPtr<ID3DBlob> pixelShader_shadow= d3dUtil::CompileShader(L"shaders\\Shadow.hlsl", nullptr, "PS_Shadow", "ps_5_1");
	
	ComPtr<ID3DBlob> vertexShader_skinned = d3dUtil::CompileShader(L"shaders\\shaders.hlsl", skinnedDefines, "VSMain", "vs_5_1");
	ComPtr<ID3DBlob> vertexShader = d3dUtil::CompileShader(L"shaders\\shaders.hlsl", nullptr, "VSMain", "vs_5_1");
	ComPtr<ID3DBlob> pixelShader = d3dUtil::CompileShader(L"shaders\\shaders.hlsl", nullptr, "PSMain", "ps_5_1");

	ComPtr<ID3DBlob> skyVS = d3dUtil::CompileShader(L"shaders\\sky.hlsl", nullptr, "VS", "vs_5_1");
	ComPtr<ID3DBlob> skyPS = d3dUtil::CompileShader(L"shaders\\sky.hlsl", nullptr, "PS", "ps_5_1");

	ComPtr<ID3DBlob> uiVS = d3dUtil::CompileShader(L"shaders\\ui.hlsl", nullptr, "VSMain", "vs_5_1");
	ComPtr<ID3DBlob> uiPS = d3dUtil::CompileShader(L"shaders\\ui.hlsl", nullptr, "PSMain", "ps_5_1");

	ComPtr<ID3DBlob> particleVS = d3dUtil::CompileShader(L"shaders\\Particle.hlsl", nullptr, "VSMain", "vs_5_1");
	ComPtr<ID3DBlob> particleGS = d3dUtil::CompileShader(L"shaders\\Particle.hlsl", nullptr, "GSMain", "gs_5_1");
	ComPtr<ID3DBlob> particlePS = d3dUtil::CompileShader(L"shaders\\Particle.hlsl", nullptr, "PSMain", "ps_5_1");
	ComPtr<ID3DBlob> particleGSMaker = d3dUtil::CompileShader(L"shaders\\Particle.hlsl", nullptr, "GSParticleMaker", "gs_5_1");

	ComPtr<ID3DBlob> grassVS = d3dUtil::CompileShader(L"shaders\\Grass.hlsl", nullptr, "VS", "vs_5_1");
	ComPtr<ID3DBlob> grassGS = d3dUtil::CompileShader(L"Shaders\\Grass.hlsl", nullptr, "GS", "gs_5_1");
	ComPtr<ID3DBlob> grassPS = d3dUtil::CompileShader(L"shaders\\Grass.hlsl", alphaTestDefines, "PS", "ps_5_1");

	ComPtr<ID3DBlob> previewVS = d3dUtil::CompileShader(L"shaders\\Preview.hlsl", nullptr, "VSMain", "vs_5_1");
	ComPtr<ID3DBlob> previewPS = d3dUtil::CompileShader(L"shaders\\Preview.hlsl", nullptr, "PSMain", "ps_5_1");


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
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_particle[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TIME", 0, DXGI_FORMAT_R32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "DIRECTION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "SPEED", 0, DXGI_FORMAT_R32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TYPE", 0, DXGI_FORMAT_R32_UINT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_ELEMENT_DESC inputElementDescs_grass[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "LOOK", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc{};
	opaquePsoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	opaquePsoDesc.pRootSignature = rootSignature.Get();
	opaquePsoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	opaquePsoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//opaquePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	opaquePsoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	opaquePsoDesc.SampleDesc.Count = 1;
	device->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&pipelineStates["opaque"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC skinnedPsoDesc = opaquePsoDesc;
	skinnedPsoDesc.InputLayout = { inputElementDescs_skinned, _countof(inputElementDescs_skinned) };
	skinnedPsoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader_skinned.Get());
	device->CreateGraphicsPipelineState(&skinnedPsoDesc, IID_PPV_ARGS(&pipelineStates["skinnedOpaque"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC skyPsoDesc = opaquePsoDesc;
	skyPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	skyPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	skyPsoDesc.VS = CD3DX12_SHADER_BYTECODE(skyVS.Get());
	skyPsoDesc.PS = CD3DX12_SHADER_BYTECODE(skyPS.Get());
	skyPsoDesc.DepthStencilState.StencilEnable = true;
	skyPsoDesc.DepthStencilState.StencilReadMask = 0x01;
	skyPsoDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	skyPsoDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	skyPsoDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	skyPsoDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL;
	skyPsoDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	device->CreateGraphicsPipelineState(&skyPsoDesc, IID_PPV_ARGS(&pipelineStates["sky"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC uiPsoDesc = opaquePsoDesc;
	uiPsoDesc.VS = CD3DX12_SHADER_BYTECODE(uiVS.Get());
	uiPsoDesc.PS = CD3DX12_SHADER_BYTECODE(uiPS.Get());
	uiPsoDesc.DepthStencilState.DepthEnable = false;
	uiPsoDesc.DepthStencilState.StencilEnable = true;
	uiPsoDesc.DepthStencilState.StencilReadMask = 0x01;
	uiPsoDesc.DepthStencilState.StencilWriteMask = 0x01;
	uiPsoDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	uiPsoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	uiPsoDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	uiPsoDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	uiPsoDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	device->CreateGraphicsPipelineState(&uiPsoDesc, IID_PPV_ARGS(&pipelineStates["ui"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC shadowPsoDesc = opaquePsoDesc;
	shadowPsoDesc.RasterizerState.DepthBias = 100000;
	shadowPsoDesc.RasterizerState.DepthBiasClamp = 0.0f;
	shadowPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
	shadowPsoDesc.pRootSignature = rootSignature.Get();
	shadowPsoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader_shadow.Get());
	shadowPsoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader_shadow.Get());
	shadowPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	shadowPsoDesc.NumRenderTargets = 0;
	device->CreateGraphicsPipelineState(&shadowPsoDesc, IID_PPV_ARGS(&pipelineStates["shadow_opaque"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC skinnedShadowPsoDesc = shadowPsoDesc;
	skinnedShadowPsoDesc.InputLayout = { inputElementDescs_skinned, _countof(inputElementDescs_skinned) };
	skinnedShadowPsoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader_skinnedShadow.Get());
	device->CreateGraphicsPipelineState(&skinnedShadowPsoDesc, IID_PPV_ARGS(&pipelineStates["shadow_skinnedOpaque"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC particlePsoDesc = opaquePsoDesc;
	particlePsoDesc.InputLayout = { inputElementDescs_particle, _countof(inputElementDescs_particle) };
	particlePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	particlePsoDesc.VS = CD3DX12_SHADER_BYTECODE(particleVS.Get());
	particlePsoDesc.GS = CD3DX12_SHADER_BYTECODE(particleGS.Get());
	particlePsoDesc.PS = CD3DX12_SHADER_BYTECODE(particlePS.Get());
	particlePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	particlePsoDesc.DepthStencilState.DepthEnable = false;
	D3D12_RENDER_TARGET_BLEND_DESC blendDesc{};
	blendDesc.BlendEnable = true;
	blendDesc.LogicOpEnable = false;
	blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	particlePsoDesc.BlendState.RenderTarget[0] = blendDesc;
	device->CreateGraphicsPipelineState(&particlePsoDesc, IID_PPV_ARGS(&pipelineStates["particle"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC particleMakerPsoDesc = opaquePsoDesc;
	particleMakerPsoDesc.InputLayout = { inputElementDescs_particle, _countof(inputElementDescs_particle) };
	particleMakerPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	particleMakerPsoDesc.VS = CD3DX12_SHADER_BYTECODE(particleVS.Get());
	particleMakerPsoDesc.GS = CD3DX12_SHADER_BYTECODE(particleGSMaker.Get());
	particleMakerPsoDesc.PS = {};
	particleMakerPsoDesc.DepthStencilState.StencilEnable = false;
	particleMakerPsoDesc.DepthStencilState.DepthEnable = false;
	particleMakerPsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	D3D12_SO_DECLARATION_ENTRY soDecl[]
	{
		{0, "POSITION", 0, 0, 3, 0},
		{0, "SIZE", 0, 0, 2, 0},
		{0, "TIME", 0, 0, 1, 0},
		{0, "DIRECTION", 0, 0, 3, 0},
		{0, "SPEED", 0, 0, 1, 0},
		{0, "TYPE", 0, 0, 1, 0}
	};
	particleMakerPsoDesc.StreamOutput.pSODeclaration = soDecl;
	particleMakerPsoDesc.StreamOutput.NumEntries = _countof(soDecl);
	particleMakerPsoDesc.StreamOutput.pBufferStrides = NULL;
	particleMakerPsoDesc.StreamOutput.NumStrides = 0;
	particleMakerPsoDesc.StreamOutput.RasterizedStream = 0;
	device->CreateGraphicsPipelineState(&particleMakerPsoDesc, IID_PPV_ARGS(&pipelineStates["particleMaker"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC grassPsoDesc = opaquePsoDesc;
	grassPsoDesc.VS = CD3DX12_SHADER_BYTECODE(grassVS.Get());
	grassPsoDesc.GS = CD3DX12_SHADER_BYTECODE(grassGS.Get());
	grassPsoDesc.PS = CD3DX12_SHADER_BYTECODE(grassPS.Get());
	grassPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	grassPsoDesc.InputLayout = { inputElementDescs_grass, _countof(inputElementDescs_grass) };
	grassPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	device->CreateGraphicsPipelineState(&grassPsoDesc, IID_PPV_ARGS(&pipelineStates["grass"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC previewPsoDesc = opaquePsoDesc;
	previewPsoDesc.VS = CD3DX12_SHADER_BYTECODE(previewVS.Get());
	previewPsoDesc.PS = CD3DX12_SHADER_BYTECODE(previewPS.Get());
	previewPsoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	device->CreateGraphicsPipelineState(&previewPsoDesc, IID_PPV_ARGS(&pipelineStates["buildPreview"]));


	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.NumDescriptors = 10;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&srvHeap));

	shadowMap->BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(srvHeap->GetCPUDescriptorHandleForHeapStart(), 1, srvDescriptorSize),
		CD3DX12_GPU_DESCRIPTOR_HANDLE(srvHeap->GetGPUDescriptorHandleForHeapStart(), 1, srvDescriptorSize),
		CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap->GetCPUDescriptorHandleForHeapStart(), 1, dsvDescriptorSize));

	WaitForPreviousFrame();
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
