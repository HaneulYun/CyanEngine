#include "pch.h"
#include "Graphics.h"

void Graphics::Initialize()
{
	InitDirect3D();
	InitDirect2D();
	LoadAssets();
	BuildResources();
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
}

void Graphics::RenderShadowMap(LightData* lightData)
{
	auto pass = lightData->lightResource[Scene::scene->frameResourceManager.currFrameResourceIndex].get()->LightCB->Resource()->GetGPUVirtualAddress();
	commandList->SetGraphicsRootConstantBufferView(4, pass);

	commandList->SetPipelineState(pipelineStates["shadow_opaque"].Get());
	for (int i = 0; i < 4; ++i)
	{
		ShadowMap* shadowMap = lightData->shadowMap[i].get();

		commandList->RSSetViewports(1, &shadowMap->Viewport());
		commandList->RSSetScissorRects(1, &shadowMap->ScissorRect());

		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(shadowMap->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

		commandList->ClearDepthStencilView(shadowMap->Dsv(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		commandList->OMSetRenderTargets(0, nullptr, false, &shadowMap->Dsv());

		unsigned int shadowMapIndex = i;
		commandList->SetGraphicsRoot32BitConstants(8, 1, &shadowMapIndex, 0);

		for (auto layer : { RenderLayer::Opaque, RenderLayer::SkinnedOpaque, })
			RenderObjects((int)layer, true);

		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(shadowMap->Resource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}

void Graphics::Render()
{
	FrameResource* currFrameResource = Scene::scene->frameResourceManager.currFrameResource;
	int currFrameResourceIndex = Scene::scene->frameResourceManager.currFrameResourceIndex;

	if (currFrameResource->Fence != 0 && fence->GetCompletedValue() < currFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
		fence->SetEventOnCompletion(currFrameResource->Fence, eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	PreRender();

	AssetResource* currAssetResource = AssetManager::Instance()->assetResource[currFrameResourceIndex].get();
	auto matBuffer = currAssetResource->MaterialBuffer->Resource();

	commandList->SetGraphicsRootShaderResourceView(7, matBuffer->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(6, GetSrvGpu(20));
	commandList->SetGraphicsRootDescriptorTable(5, GetSrvGpu(10));
	commandList->SetGraphicsRootDescriptorTable(3, GetSrvGpu(0));

	D3D12_CPU_DESCRIPTOR_HANDLE mrt[]{ heapManager.GetRtv(2), heapManager.GetRtv(3) };
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle{ GetDsv(0) };

	const float rtClearColor[]{ 0, 0, 0, 1 };
	commandList->ClearRenderTargetView(heapManager.GetRtv(2), rtClearColor, 0, nullptr);
	commandList->ClearRenderTargetView(heapManager.GetRtv(3), rtClearColor, 0, nullptr);

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

	commandList->SetGraphicsRootConstantBufferView(4, passCB->GetGPUVirtualAddress());

	commandList->OMSetRenderTargets(_countof(mrt), mrt, FALSE, nullptr);
	for (auto layer : { RenderLayer::Sky })
		RenderObjects((int)layer);

	commandList->OMSetRenderTargets(_countof(mrt), mrt, FALSE, &dsvHandle);
	for (auto layer : {
		RenderLayer::Opaque, RenderLayer::SkinnedOpaque, RenderLayer::Grass,
		RenderLayer::BuildPreview, RenderLayer::Particle, })
		RenderObjects((int)layer);


	commandList->ClearRenderTargetView(heapManager.GetRtv(4), rtClearColor, 0, nullptr);
	commandList->ClearRenderTargetView(heapManager.GetRtv(5), rtClearColor, 0, nullptr);

	for (int lightType = 0; lightType < Light::Type::Count; ++lightType)
	{
		for (auto light : Scene::scene->lightResourceManager.lightObjects[lightType])
		{

			if (light->gameObject->GetComponent<Light>()->shadowType)
				RenderShadowMap(light);

			D3D12_CPU_DESCRIPTOR_HANDLE mrt[]{ heapManager.GetRtv(4), heapManager.GetRtv(5) };
			commandList->OMSetRenderTargets(_countof(mrt), mrt, FALSE, nullptr);
			commandList->RSSetViewports(1, &viewport);
			commandList->RSSetScissorRects(1, &scissorRect);

			auto gameObject = light->gameObject;
			Matrix4x4 worldMatrix = gameObject->GetMatrix();
			PassLight l = PassLight{ gameObject->GetComponent<Light>()->get(worldMatrix.forward, worldMatrix.position) };
			commandList->SetGraphicsRoot32BitConstants(8, 16, &l, 0);
			commandList->SetGraphicsRootDescriptorTable(9, GetSrvGpu(15));

			switch (lightType)
			{
			case Light::Type::Directional:
				commandList->SetPipelineState(pipelineStates["directional"].Get());
				commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				commandList->DrawInstanced(4, 1, 0, 0);
				break;
			case Light::Type::Point:
				commandList->SetPipelineState(pipelineStates["point"].Get());
				commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
				commandList->DrawInstanced(1, 1, 0, 0);
				break;
			case Light::Type::Spot:
				commandList->SetPipelineState(pipelineStates["spot"].Get());
				commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
				commandList->DrawInstanced(1, 1, 0, 0);
				break;
			}
		}
	}

	commandList->SetGraphicsRootConstantBufferView(4, passCB->GetGPUVirtualAddress());

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	
	const float clearColor[]{ 0.1921569, 0.3019608, 0.4745098, 1.0f };
	commandList->ClearRenderTargetView(heapManager.GetRtv(frameIndex), clearColor, 0, nullptr);
	commandList->OMSetRenderTargets(1, &heapManager.GetRtv(frameIndex), FALSE, nullptr);

	commandList->SetPipelineState(pipelineStates["deferred"].Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	commandList->DrawInstanced(4, 1, 0, 0);

	if (isShadowDebug)
	{
		commandList->SetPipelineState(pipelineStates["debug"].Get());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		commandList->DrawInstanced(4, 10, 0, 0);
	}

	for (auto layer : { RenderLayer::UI })
		RenderObjects((int)layer);

	PostRender();
}

void Graphics::RenderObjects(int layerIndex, bool isShadowMap)
{
	if (isShadowMap)
	{
		if (layerIndex == (int)RenderLayer::SkinnedOpaque)
			commandList->SetPipelineState(pipelineStates["shadow_skinnedOpaque"].Get());
		else
			commandList->SetPipelineState(pipelineStates["shadow_opaque"].Get());
	}
	else
	{
		if (layerIndex == (int)RenderLayer::SkinnedOpaque)
			commandList->SetPipelineState(pipelineStates["skinnedOpaque"].Get());
		else if (layerIndex == (int)RenderLayer::Sky)
		{
			commandList->SetPipelineState(pipelineStates["sky"].Get());
			if (Scene::scene->environment.sunSources)
			{
				auto light = Scene::scene->environment.sunSources;
				Matrix4x4 worldMatrix = light->gameObject->GetMatrix();
				PassLight l = PassLight{ light->get(worldMatrix.forward, worldMatrix.position) };
				commandList->SetGraphicsRoot32BitConstants(8, 16, &l, 0);
			}
		}
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
		if (!mesh)
			continue;
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
			commandList->SetGraphicsRoot32BitConstants(8, 4, &v.xmf4, 0);
		}
		commandList->SetGraphicsRootShaderResourceView(0, instanceBuffer->Resource()->GetGPUVirtualAddress());
		commandList->SetGraphicsRootShaderResourceView(2, skinnedBuffer->Resource()->GetGPUVirtualAddress());

		commandList->IASetPrimitiveTopology(mesh->PrimitiveType);

		int i = 0;
		if (layerIndex != (int)RenderLayer::Particle)
		{
			commandList->IASetVertexBuffers(0, 1, &mesh->VertexBufferView());
			if (mesh->IndexBufferByteSize)
				commandList->IASetIndexBuffer(&mesh->IndexBufferView());
			for (auto& submesh : mesh->DrawArgs)
			{
				commandList->SetGraphicsRootShaderResourceView(1, matIndexBuffer->Resource()->GetGPUVirtualAddress() + sizeof(MatIndexData) * i++);
				if (mesh->IndexBufferByteSize)
					commandList->DrawIndexedInstanced(submesh.second.IndexCount, objects.size(), submesh.second.StartIndexLocation, submesh.second.BaseVertexLocation, 0);
				else
					commandList->DrawInstanced(submesh.second.IndexCount, objects.size(), submesh.second.StartIndexLocation, 0);
			}

			if (auto terrain = objects[0]->GetComponent<Terrain>(); terrain && !isShadowMap)
			{
				auto detail = terrain->terrainData.detailPrototype;
				auto mesh = detail.mesh;
				auto material = detail.material;

				commandList->SetPipelineState(pipelineStates["grass"].Get());
				commandList->IASetPrimitiveTopology(mesh->PrimitiveType);
				commandList->IASetVertexBuffers(0, 1, &mesh->VertexBufferView());
				int j = 0;
				for (auto& submesh : mesh->DrawArgs)
				{
					commandList->SetGraphicsRootShaderResourceView(1, matIndexBuffer->Resource()->GetGPUVirtualAddress() + sizeof(MatIndexData) * 1);
					commandList->DrawInstanced(submesh.second.IndexCount, objects.size(), submesh.second.StartIndexLocation, 0);
				}
				commandList->SetPipelineState(pipelineStates["opaque"].Get());
			}
		}
		else
		{
			commandList->SetGraphicsRootShaderResourceView(1, matIndexBuffer->Resource()->GetGPUVirtualAddress());
			auto temp = mesh;
			auto mesh = (ParticleBundle*)temp;
			commandList->SetPipelineState(pipelineStates["particleMaker"].Get());

			ParticleResource* res = mesh->particleResource[Scene::scene->frameResourceManager.currFrameResourceIndex % 3];
			ParticleResource* resource = mesh->particleResource[(Scene::scene->frameResourceManager.currFrameResourceIndex + 1) % 3];
			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource->VertexParticleBufferGPU.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
			commandList->CopyResource(resource->VertexParticleBufferGPU.Get(), mesh->VertexParticleInitBufferGPU.Get());
			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource->VertexParticleBufferGPU.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_STREAM_OUT));

			commandList->SOSetTargets(0, 1, &mesh->StreamOutputBufferView());
			commandList->IASetVertexBuffers(0, 1, &mesh->VertexBufferView());
			commandList->DrawInstanced(res->particleCount, objects.size(), 0, 0);
			commandList->SOSetTargets(0, 1, &mesh->StreamOutputBufferView());

			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource->VertexParticleBufferGPU.Get(), D3D12_RESOURCE_STATE_STREAM_OUT, D3D12_RESOURCE_STATE_COPY_SOURCE));
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

		if (auto rt = gameObject->GetComponent<RectTransform>(); rt)
			if (rt->renderMode)
			{
				auto v = Vector3(0).TransformCoord(gameObject->parent->GetMatrix()* Camera::main->view * Camera::main->projection);
				RectTransform r;
				r.width = rect->width;
				r.height = rect->height;
				r.setAnchorAndPivot((v.x + 1) / 2, (v.y + 1) / 2);
				r.Update();
				rect->UpdateTransform(r.width, r.height);

				mat = RectTransform::Transform(gameObject->GetMatrix(gameObject) * r.localToWorldMatrix);
				//mat = RectTransform::Transform(gameObject->GetMatrix(gameObject));
			}


		Vector3 leftTop{ mat._41, mat._22 + mat._42, 0 };
		Vector3 rightBottom{ mat._11 + mat._41, mat._42, 0 };

		rt.left = (leftTop.x / 2.0f + 0.5f) * width;
		rt.top = (leftTop.y / -2.0f + 0.5f) * height;
		rt.right = (rightBottom.x / 2.0f + 0.5f) * width;
		rt.bottom = (rightBottom.y / -2.0f + 0.5f) * height;


		if (textComponent->gameObject->active)
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

	WaitForPreviousFrame();

	for (auto& renderSets : Scene::scene->objectRenderManager.renderObjectsLayer[(int)RenderLayer::Particle])
	{
		auto mesh = (ParticleBundle*)renderSets.first;

		UINT8* p;
		D3D12_RANGE range{ 0, sizeof(UINT64*) };
		mesh->VertexParticleBufferReadback->Map(0, &range, reinterpret_cast<void**>(&p));

		UINT64 size = *reinterpret_cast<UINT64*>(p);
		FrameResource::ParticleSpriteVertex* t = reinterpret_cast<FrameResource::ParticleSpriteVertex*>(p + 8);

		ParticleResource* resource = mesh->particleResource[(Scene::scene->frameResourceManager.currFrameResourceIndex + 1) % 3];
		resource->particleCount = size / sizeof(FrameResource::ParticleSpriteVertex);

		mesh->VertexParticleBufferReadback->Unmap(0, nullptr);
	}
}

void Graphics::Destroy()
{
	WaitForPreviousFrame();

	swapChain->SetFullscreenState(FALSE, NULL);
}

void Graphics::InitDirect3D()
{
	UINT dxgiFactoryFlags{ 0 };

#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debug{ nullptr };
	D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	if (debug)
		debug->EnableDebugLayer();

	debug.Reset();
	debug.ReleaseAndGetAddressOf();

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
	factory->CreateSwapChainForHwnd(commandQueue.Get(), CyanApp::GetHwnd(), &swapChainDesc, nullptr, nullptr, (IDXGISwapChain1 * *)swapChain.GetAddressOf());
	factory->MakeWindowAssociation(CyanApp::GetHwnd(), DXGI_MWA_NO_ALT_ENTER);
	frameIndex = swapChain->GetCurrentBackBufferIndex();
	
	
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.NumDescriptors = FrameCount + 4;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	//device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&rtvHeap));
	heapManager.BuildRtvHeap(device.Get(), FrameCount + 4);
	
	
	descriptorHeapDesc.NumDescriptors = 1+4;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&dsvHeap));
	
	CreateRenderTargetView();
	
	//D3D12_RESOURCE_DESC resourceDesc{};
	//resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//resourceDesc.Alignment = 0;
	//resourceDesc.Width = CyanFW::Instance()->GetWidth();
	//resourceDesc.Height = CyanFW::Instance()->GetHeight();;
	//resourceDesc.DepthOrArraySize = 1;
	//resourceDesc.MipLevels = 1;
	//resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//resourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	//resourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	//resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	//resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	
	//D3D12_CLEAR_VALUE d3dClearValue;
	//d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//d3dClearValue.DepthStencil.Depth = 1.0f;
	//d3dClearValue.DepthStencil.Stencil = 0;
	//device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &resourceDesc,
	//	D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, IID_PPV_ARGS(&depthStencilBuffer));
	//
	//device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, GetDsv(0));
}

void Graphics::InitDirect2D()
{
	// Create an 11 device wrapped around the 12 device and share
	// 12's command queue.

	UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};

	ComPtr<ID3D11Device> d3d11Device;
	D3D11On12CreateDevice(device.Get(), d3d11DeviceFlags, nullptr, 0, reinterpret_cast<IUnknown * *>(commandQueue.GetAddressOf()),
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

	CD3DX12_DESCRIPTOR_RANGE texTable0;
	texTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 8, 1, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE texTable1;
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE texTable2;
	texTable2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 256 - 20, 0, 4, 0);

	CD3DX12_DESCRIPTOR_RANGE texTable3;
	texTable3.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0, 3, 0);


	CD3DX12_ROOT_PARAMETER rootParameters[10];
	rootParameters[0].InitAsShaderResourceView(0);
	rootParameters[1].InitAsShaderResourceView(1);
	rootParameters[2].InitAsShaderResourceView(2);
	rootParameters[3].InitAsDescriptorTable(1, &texTable0, D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[4].InitAsConstantBufferView(0);
	rootParameters[5].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[6].InitAsDescriptorTable(1, &texTable2, D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[7].InitAsShaderResourceView(0, 2);
	rootParameters[8].InitAsConstants(16, 1);
	rootParameters[9].InitAsDescriptorTable(1, &texTable3, D3D12_SHADER_VISIBILITY_PIXEL);

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
		//D3D12_ROOT_SIGNATURE_FLAG_ALLOW_HULL_SHADER_ROOT_ACCESS |
		//D3D12_ROOT_SIGNATURE_FLAG_ALLOW_DOMAIN_SHADER_ROOT_ACCESS |
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

	ComPtr<ID3DBlob> skinnedShadowVS = d3dUtil::CompileShader(L"shaders\\shadow.hlsl", skinnedDefines, "VS", "vs_5_1");
	ComPtr<ID3DBlob> shadowVS = d3dUtil::CompileShader(L"shaders\\shadow.hlsl", nullptr, "VS", "vs_5_1");
	ComPtr<ID3DBlob> shadowPS = d3dUtil::CompileShader(L"shaders\\shadow.hlsl", nullptr, "PS", "ps_5_1");

	ComPtr<ID3DBlob> skinnedVS = d3dUtil::CompileShader(L"shaders\\shaders.hlsl", skinnedDefines, "VS", "vs_5_1");
	ComPtr<ID3DBlob> opaqueVS = d3dUtil::CompileShader(L"shaders\\shaders.hlsl", nullptr, "VS", "vs_5_1");
	ComPtr<ID3DBlob> opaquePS = d3dUtil::CompileShader(L"shaders\\shaders.hlsl", nullptr, "PS", "ps_5_1");

	ComPtr<ID3DBlob> skyVS = d3dUtil::CompileShader(L"shaders\\sky.hlsl", nullptr, "VS", "vs_5_1");
	ComPtr<ID3DBlob> skyPS = d3dUtil::CompileShader(L"shaders\\sky.hlsl", nullptr, "PS", "ps_5_1");

	ComPtr<ID3DBlob> uiVS = d3dUtil::CompileShader(L"shaders\\ui.hlsl", nullptr, "VS", "vs_5_1");
	ComPtr<ID3DBlob> uiPS = d3dUtil::CompileShader(L"shaders\\ui.hlsl", nullptr, "PS", "ps_5_1");

	ComPtr<ID3DBlob> particleVS = d3dUtil::CompileShader(L"shaders\\particle.hlsl", nullptr, "VS", "vs_5_1");
	ComPtr<ID3DBlob> particleGS = d3dUtil::CompileShader(L"shaders\\particle.hlsl", nullptr, "GS", "gs_5_1");
	ComPtr<ID3DBlob> particlePS = d3dUtil::CompileShader(L"shaders\\particle.hlsl", nullptr, "PS", "ps_5_1");
	ComPtr<ID3DBlob> particleSO = d3dUtil::CompileShader(L"shaders\\particle.hlsl", nullptr, "SO", "gs_5_1");

	ComPtr<ID3DBlob> grassVS = d3dUtil::CompileShader(L"shaders\\grass.hlsl", nullptr, "VS", "vs_5_1");
	ComPtr<ID3DBlob> grassGS = d3dUtil::CompileShader(L"Shaders\\grass.hlsl", nullptr, "GS", "gs_5_1");
	ComPtr<ID3DBlob> grassPS = d3dUtil::CompileShader(L"shaders\\grass.hlsl", alphaTestDefines, "PS", "ps_5_1");

	ComPtr<ID3DBlob> previewVS = d3dUtil::CompileShader(L"shaders\\preview.hlsl", nullptr, "VS", "vs_5_1");
	ComPtr<ID3DBlob> previewPS = d3dUtil::CompileShader(L"shaders\\preview.hlsl", nullptr, "PS", "ps_5_1");

	ComPtr<ID3DBlob> debugVS = d3dUtil::CompileShader(L"shaders\\shadowDebug.hlsl", nullptr, "VS", "vs_5_1");
	ComPtr<ID3DBlob> debugPS = d3dUtil::CompileShader(L"shaders\\shadowDebug.hlsl", nullptr, "PS", "ps_5_1");

	ComPtr<ID3DBlob> directionalVS = d3dUtil::CompileShader(L"shaders\\light.hlsl", nullptr, "VS", "vs_5_1");
	ComPtr<ID3DBlob> directionalPS = d3dUtil::CompileShader(L"shaders\\light.hlsl", nullptr, "PS", "ps_5_1");

	ComPtr<ID3DBlob> pointVS = d3dUtil::CompileShader(L"shaders\\light.hlsl", nullptr, "pointVS", "vs_5_1");
	ComPtr<ID3DBlob> pointHS = d3dUtil::CompileShader(L"shaders\\light.hlsl", nullptr, "HS", "hs_5_1");
	ComPtr<ID3DBlob> pointDS = d3dUtil::CompileShader(L"shaders\\light.hlsl", nullptr, "DS", "ds_5_1");

	ComPtr<ID3DBlob> spotDS = d3dUtil::CompileShader(L"shaders\\light.hlsl", nullptr, "spotDS", "ds_5_1");

	ComPtr<ID3DBlob> deferredVS = d3dUtil::CompileShader(L"shaders\\deferred.hlsl", nullptr, "VS", "vs_5_1");
	ComPtr<ID3DBlob> deferredPS = d3dUtil::CompileShader(L"shaders\\deferred.hlsl", nullptr, "PS", "ps_5_1");


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
		{ "LOOK", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc{};
	opaquePsoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	opaquePsoDesc.pRootSignature = rootSignature.Get();
	opaquePsoDesc.VS = CD3DX12_SHADER_BYTECODE(opaqueVS.Get());
	opaquePsoDesc.PS = CD3DX12_SHADER_BYTECODE(opaquePS.Get());
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
	skinnedPsoDesc.VS = CD3DX12_SHADER_BYTECODE(skinnedVS.Get());
	device->CreateGraphicsPipelineState(&skinnedPsoDesc, IID_PPV_ARGS(&pipelineStates["skinnedOpaque"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC debugPsoDesc = opaquePsoDesc;
	debugPsoDesc.VS = CD3DX12_SHADER_BYTECODE(debugVS.Get());
	debugPsoDesc.PS = CD3DX12_SHADER_BYTECODE(debugPS.Get());
	device->CreateGraphicsPipelineState(&debugPsoDesc, IID_PPV_ARGS(&pipelineStates["debug"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC directionalPsoDesc = opaquePsoDesc;
	directionalPsoDesc.VS = CD3DX12_SHADER_BYTECODE(directionalVS.Get());
	directionalPsoDesc.PS = CD3DX12_SHADER_BYTECODE(directionalPS.Get());
	directionalPsoDesc.DepthStencilState = D3D12_DEPTH_STENCIL_DESC{};
	directionalPsoDesc.NumRenderTargets = 2;
	directionalPsoDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	directionalPsoDesc.RTVFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	directionalPsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	directionalPsoDesc.BlendState.RenderTarget[0].BlendEnable = true;
	directionalPsoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	directionalPsoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	directionalPsoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	directionalPsoDesc.BlendState.RenderTarget[1] = directionalPsoDesc.BlendState.RenderTarget[0];
	device->CreateGraphicsPipelineState(&directionalPsoDesc, IID_PPV_ARGS(&pipelineStates["directional"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pointPsoDesc = directionalPsoDesc;
	pointPsoDesc.VS = CD3DX12_SHADER_BYTECODE(pointVS.Get());
	pointPsoDesc.HS = CD3DX12_SHADER_BYTECODE(pointHS.Get());
	pointPsoDesc.DS = CD3DX12_SHADER_BYTECODE(pointDS.Get());
	pointPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	pointPsoDesc.DepthStencilState = D3D12_DEPTH_STENCIL_DESC{};
	pointPsoDesc.DepthStencilState.DepthEnable = true;
	pointPsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	pointPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	pointPsoDesc.RasterizerState = D3D12_RASTERIZER_DESC{};
	pointPsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pointPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	device->CreateGraphicsPipelineState(&pointPsoDesc, IID_PPV_ARGS(&pipelineStates["point"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC spotPsoDesc = pointPsoDesc;
	spotPsoDesc.DS = CD3DX12_SHADER_BYTECODE(spotDS.Get());
	spotPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
	//spotPsoDesc.DepthStencilState.DepthEnable = false;
	//spotPsoDesc.RasterizerState = D3D12_RASTERIZER_DESC{};
	//spotPsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//spotPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
	device->CreateGraphicsPipelineState(&spotPsoDesc, IID_PPV_ARGS(&pipelineStates["spot"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC deferredPsoDesc = opaquePsoDesc;
	deferredPsoDesc.VS = CD3DX12_SHADER_BYTECODE(deferredVS.Get());
	deferredPsoDesc.PS = CD3DX12_SHADER_BYTECODE(deferredPS.Get());
	device->CreateGraphicsPipelineState(&deferredPsoDesc, IID_PPV_ARGS(&pipelineStates["deferred"]));

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
	uiPsoDesc.DepthStencilState.DepthEnable = false;
	uiPsoDesc.BlendState.RenderTarget[0] = blendDesc;
	device->CreateGraphicsPipelineState(&uiPsoDesc, IID_PPV_ARGS(&pipelineStates["ui"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC shadowPsoDesc = opaquePsoDesc;
	shadowPsoDesc.RasterizerState.DepthBias = 100000;
	shadowPsoDesc.RasterizerState.DepthBiasClamp = 0.0f;
	shadowPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
	shadowPsoDesc.pRootSignature = rootSignature.Get();
	shadowPsoDesc.VS = CD3DX12_SHADER_BYTECODE(shadowVS.Get());
	shadowPsoDesc.PS = CD3DX12_SHADER_BYTECODE(shadowPS.Get());
	shadowPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	shadowPsoDesc.NumRenderTargets = 0;
	shadowPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	device->CreateGraphicsPipelineState(&shadowPsoDesc, IID_PPV_ARGS(&pipelineStates["shadow_opaque"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC skinnedShadowPsoDesc = shadowPsoDesc;
	skinnedShadowPsoDesc.InputLayout = { inputElementDescs_skinned, _countof(inputElementDescs_skinned) };
	skinnedShadowPsoDesc.VS = CD3DX12_SHADER_BYTECODE(skinnedShadowVS.Get());
	device->CreateGraphicsPipelineState(&skinnedShadowPsoDesc, IID_PPV_ARGS(&pipelineStates["shadow_skinnedOpaque"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC particlePsoDesc = opaquePsoDesc;
	particlePsoDesc.InputLayout = { inputElementDescs_particle, _countof(inputElementDescs_particle) };
	particlePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	particlePsoDesc.VS = CD3DX12_SHADER_BYTECODE(particleVS.Get());
	particlePsoDesc.GS = CD3DX12_SHADER_BYTECODE(particleGS.Get());
	particlePsoDesc.PS = CD3DX12_SHADER_BYTECODE(particlePS.Get());
	particlePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	particlePsoDesc.DepthStencilState.DepthEnable = false;
	particlePsoDesc.BlendState.RenderTarget[0] = blendDesc;
	device->CreateGraphicsPipelineState(&particlePsoDesc, IID_PPV_ARGS(&pipelineStates["particle"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC particleMakerPsoDesc = opaquePsoDesc;
	particleMakerPsoDesc.InputLayout = { inputElementDescs_particle, _countof(inputElementDescs_particle) };
	particleMakerPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	particleMakerPsoDesc.VS = CD3DX12_SHADER_BYTECODE(particleVS.Get());
	particleMakerPsoDesc.GS = CD3DX12_SHADER_BYTECODE(particleSO.Get());
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
	descriptorHeapDesc.NumDescriptors = 256;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&srvHeap));

	WaitForPreviousFrame();
}

//--------------//

void Graphics::ChangeSwapChainState()
{
	WaitForPreviousFrame();

	BOOL bFullScreenState = FALSE;
	swapChain->GetFullscreenState(&bFullScreenState, NULL);

	bFullScreenState = !bFullScreenState;
	swapChain->SetFullscreenState(bFullScreenState, NULL);

	int w, h;
	if (bFullScreenState)
	{
		//DXGI_SWAP_CHAIN_DESC1 desc;
		//swapChain->GetDesc1(&desc);
		//int w = desc.Width;
		//int h = desc.Height;
		w = 1920;
		h = 1080;
	}
	else
	{
		w = CyanFW::Instance()->window.x;
		h = CyanFW::Instance()->window.y;
	}
	
	CyanFW::Instance()->SetWidth(w);
	CyanFW::Instance()->SetHeight(h);
	BuildResources();

	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = CyanFW::Instance()->GetWidth();
	dxgiTargetParameters.Height = CyanFW::Instance()->GetHeight();;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChain->ResizeTarget(&dxgiTargetParameters);

	{
		textFormats.clear();
		textBrushes.clear();
		writeFactory.Reset();

		renderTargets2d[1].Reset();
		renderTargets2d[0].Reset();
		deviceContext.Reset();
		d2dDevice.Reset();

		d11DeviceContext.Reset();
		wrappedBackBuffers[1].Reset();
		wrappedBackBuffers[0].Reset();
		device11On12.Reset();
	}

	for (GameObject* gameObject : Scene::scene->textObjects)
	{
		Text* textComponent = gameObject->GetComponent<Text>();
		if (textComponent)
		{
			textComponent->formatIndex = -1;
			textComponent->brushIndex = -1;
		}
	}

	for (int i = 0; i < FrameCount; i++)
		if (renderTargets[i])
			renderTargets[i].Reset();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	swapChain->GetDesc(&dxgiSwapChainDesc);
	swapChain->ResizeBuffers(FrameCount, CyanFW::Instance()->GetWidth(), CyanFW::Instance()->GetHeight(), dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetView();
	InitDirect2D();
}

void Graphics::CreateRenderTargetView()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ heapManager.GetRtv(0) };
	for (UINT i = 0; i < FrameCount; ++i)
	{
		swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
		device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);

		rtvHandle.Offset(1, rtvDescriptorSize);
	}
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

void Graphics::BuildResources()
{
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = CyanFW::Instance()->GetWidth();
	resourceDesc.Height = CyanFW::Instance()->GetHeight();;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	resourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;
	device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, IID_PPV_ARGS(&depthStencilBuffer));

	device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, GetDsv(0));

	static const DXGI_FORMAT NormalMapFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = CyanFW::Instance()->GetWidth();
	texDesc.Height = CyanFW::Instance()->GetHeight();
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = NormalMapFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	float normalClearColor[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	CD3DX12_CLEAR_VALUE optClear(NormalMapFormat, normalClearColor);
	device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		&texDesc, D3D12_RESOURCE_STATE_GENERIC_READ, &optClear, IID_PPV_ARGS(&diffuseMap));

	optClear.Color[2] = { 0.0f };
	device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		&texDesc, D3D12_RESOURCE_STATE_GENERIC_READ, &optClear, IID_PPV_ARGS(&normalMap));
	device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		&texDesc, D3D12_RESOURCE_STATE_GENERIC_READ, &optClear, IID_PPV_ARGS(&lightDiffuse));
	device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		&texDesc, D3D12_RESOURCE_STATE_GENERIC_READ, &optClear, IID_PPV_ARGS(&lightSpecular));


	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = NormalMapFormat;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;
	device->CreateRenderTargetView(diffuseMap.Get(), &rtvDesc, heapManager.GetRtv(2));
	device->CreateRenderTargetView(normalMap.Get(), &rtvDesc, heapManager.GetRtv(3));
	device->CreateRenderTargetView(lightDiffuse.Get(), &rtvDesc, heapManager.GetRtv(4));
	device->CreateRenderTargetView(lightSpecular.Get(), &rtvDesc, heapManager.GetRtv(5));


	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(depthStencilBuffer.Get(), &srvDesc, GetSrv(0));

	srvDesc.Format = NormalMapFormat;
	device->CreateShaderResourceView(diffuseMap.Get(), &srvDesc, GetSrv(1));
	device->CreateShaderResourceView(normalMap.Get(), &srvDesc, GetSrv(2));
	device->CreateShaderResourceView(lightDiffuse.Get(), &srvDesc, GetSrv(3));
	device->CreateShaderResourceView(lightSpecular.Get(), &srvDesc, GetSrv(4));
}

CD3DX12_CPU_DESCRIPTOR_HANDLE Graphics::GetSrv(int index) const
{
	auto srv = CD3DX12_CPU_DESCRIPTOR_HANDLE(srvHeap->GetCPUDescriptorHandleForHeapStart());
	srv.Offset(index, srvDescriptorSize);
	return srv;
}

//CD3DX12_CPU_DESCRIPTOR_HANDLE Graphics::GetRtv(int index) const
//{
//	auto rtv = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart());
//	rtv.Offset(index, rtvDescriptorSize);
//	return rtv;
//}

CD3DX12_CPU_DESCRIPTOR_HANDLE Graphics::GetDsv(int index) const
{
	auto dsv = CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap->GetCPUDescriptorHandleForHeapStart());
	dsv.Offset(index, dsvDescriptorSize);
	return dsv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE Graphics::GetSrvGpu(int index)const
{
	auto srv = CD3DX12_GPU_DESCRIPTOR_HANDLE(srvHeap->GetGPUDescriptorHandleForHeapStart());
	srv.Offset(index, srvDescriptorSize);
	return srv;
}