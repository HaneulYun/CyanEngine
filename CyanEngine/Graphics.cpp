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

	// UpdateObjectCBs
	auto currObjectCB = currFrameResource->ObjectCB.get();
	auto currSkinnedCB = currFrameResource->SkinnedCB.get();
	auto currMatIndexBuffer = currFrameResource->MatIndexBuffer.get();
	for (auto& e : Scene::scene->gameObjects)
	{
		if (e->NumFramesDirty > 0)
		{
			XMMATRIX world;
			auto transform = e->GetComponent<Transform>();
			if (transform)
				world = XMLoadFloat4x4(&transform->localToWorldMatrix);

			XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);

			InstanceData objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
			objConstants.MaterialIndexStride = 0;
			if (e->GetComponent<SkinnedMeshRenderer>())
				objConstants.MaterialIndexStride = e->GetComponent<SkinnedMeshRenderer>()->materials.size();

			objConstants.BoneTransformStride = 0;
			if (e->GetComponent<Animator>())
				objConstants.BoneTransformStride = e->GetComponent<Animator>()->controller->BoneCount();

			if(e->ObjCBIndex != -1)
				currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			--e->NumFramesDirty;
		}
		if (e->GetComponent<Animator>())
		{
			int base = e->ObjCBIndex * e->GetComponent<Animator>()->controller->BoneCount();

			e->GetComponent<Animator>()->UpdateSkinnedAnimation(Time::deltaTime);

			for (int i = 0; i < e->GetComponent<Animator>()->FinalTransforms.size(); ++i)
			{
				SkinnnedData skinnedConstants;
				skinnedConstants.BoneTransforms = e->GetComponent<Animator>()->FinalTransforms[i];
				currSkinnedCB->CopyData(base + i, skinnedConstants);
			}
		}
		if (e->GetComponent<SkinnedMeshRenderer>())
		{
			int base = e->ObjCBIndex * e->GetComponent<SkinnedMeshRenderer>()->materials.size();

			for (int i = 0; i < e->GetComponent<SkinnedMeshRenderer>()->materials.size(); ++i)
			{
				MatIndexData skinnedConstants;
				skinnedConstants.MaterialIndex = e->GetComponent<SkinnedMeshRenderer>()->materials[i];
				currMatIndexBuffer->CopyData(base + i, skinnedConstants);
			}
		}
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
	commandList->SetGraphicsRootShaderResourceView(5, currFrameResource->ObjectCB->Resource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(6, currFrameResource->SkinnedCB->Resource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(4, srvHeap->GetGPUDescriptorHandleForHeapStart());

	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(InstanceData));
	UINT skinnedCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(SkinnnedData));
	auto objectCB = currFrameResource->ObjectCB->Resource();
	auto skinnedCB = currFrameResource->SkinnedCB->Resource();

	for(int layerIndex = 0; layerIndex < (int)RenderLayer::Count; ++layerIndex)
	{
		for(auto& renderSets: Scene::scene->renderObjectsLayer[layerIndex])
		{
			auto& mesh = renderSets.first;
			auto& objects = renderSets.second.gameObjects;
			if (!objects.size())
				continue;

			if (layerIndex == (int)RenderLayer::SkinnedOpaque)
				commandList->SetPipelineState(pipelineStates["skinnedOpaque"].Get());
			else
				commandList->SetPipelineState(pipelineStates["opaque"].Get());

			commandList->IASetVertexBuffers(0, 1, &mesh->VertexBufferView());
			commandList->IASetIndexBuffer(&mesh->IndexBufferView());
			commandList->IASetPrimitiveTopology(mesh->PrimitiveType);

			int i = 0;
			for (auto& submesh : mesh->DrawArgs)
			{
				commandList->SetGraphicsRootShaderResourceView(7,
					currFrameResource->MatIndexBuffer->Resource()->GetGPUVirtualAddress()
					+ sizeof(MatIndexData) * i++);
				commandList->DrawIndexedInstanced(
					submesh.second.IndexCount, objects.size(),
					submesh.second.StartIndexLocation,
					submesh.second.BaseVertexLocation, 0);
			}
		}
	}
	PostRender();
}


void Graphics::RenderUI()
{
	UINT width = CyanFW::Instance()->GetWidth();
	UINT height = CyanFW::Instance()->GetHeight();

	// Acquire our wrapped render target resource for the current back buffer.
	device11On12->AcquireWrappedResources(wrappedBackBuffers[frameIndex].GetAddressOf(), 1);

	// Render text directly to the back buffer.
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
				deviceContext->CreateSolidColorBrush(textComponent->color, &textBrush);
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
					textFormats[i]->GetTextAlignment() == textComponent->alignment &&
					textFormats[i]->GetFontWeight() == textComponent->weight &&
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
					DWRITE_FONT_WEIGHT(textComponent->weight),
					DWRITE_FONT_STYLE(textComponent->style),
					DWRITE_FONT_STRETCH_NORMAL,
					textComponent->fontSize,
					L"ko-KR",
					&textFormat
				);
				textComponent->formatIndex = textFormats.size();
				textFormats.push_back(textFormat);
			}
		}

		deviceContext->DrawText(
			textComponent->text.c_str(),
			textComponent->text.length(),
			textFormats[textComponent->formatIndex].Get(),
			&D2D1::RectF(textComponent->textBox.x * width, textComponent->textBox.y * height, textComponent->textBox.z * width, textComponent->textBox.w * height),
			textBrushes[textComponent->brushIndex].Get()
		);
	}

	deviceContext->EndDraw();

	// Release our wrapped render target resource. Releasing 
	// transitions the back buffer resource to the state specified
	// as the OutState when the wrapped resource was created.
	device11On12->ReleaseWrappedResources(wrappedBackBuffers[frameIndex].GetAddressOf(), 1);

	// Flush to submit the 11 command list to the shared command queue.
	d11DeviceContext->Flush();
}


void Graphics::PostRender()
{
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	commandList->Close();

	ID3D12CommandList* ppd3dCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists), ppd3dCommandLists);

	RenderUI();
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

	// Create an 11 device wrapped around the 12 device and share
	// 12's command queue.

	UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};

	ComPtr<ID3D11Device> d3d11Device;
	D3D11On12CreateDevice(
		device.Get(),
		d3d11DeviceFlags,
		nullptr,
		0,
		reinterpret_cast<IUnknown**>(commandQueue.GetAddressOf()),
		1,
		0,
		&d3d11Device,
		&d11DeviceContext,
		nullptr
	);

	// Query the 11On12 device from the 11 device.
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
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpiX,
		dpiY
	);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT i = 0; i < FrameCount; ++i)
	{
		swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
		device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);

		D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
		device11On12->CreateWrappedResource(
			renderTargets[i].Get(),
			&d3d11Flags,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT,
			IID_PPV_ARGS(&wrappedBackBuffers[i])
		);

		ComPtr<IDXGISurface> surface;
		wrappedBackBuffers[i].As(&surface);
		deviceContext->CreateBitmapFromDxgiSurface(
			surface.Get(),
			&bitmapProperties,
			&renderTargets2d[i]
		);

		rtvHandle.Offset(1, rtvDescriptorSize);
	}

}

void Graphics::LoadAssets()
{
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1;

	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 9, 0, 0, 0);

	CD3DX12_ROOT_PARAMETER rootParameters[8];
	rootParameters[0].InitAsConstantBufferView(0);
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
