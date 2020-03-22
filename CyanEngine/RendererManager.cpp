#include "pch.h"
#include "RendererManager.h"

extern UINT gnCbvSrvDescriptorIncrementSize;

struct BoneWeightData
{
	unsigned int boneIndex;
	double weight;
};

std::map<std::string, int> skeletonIndexer;
std::map<int, std::vector<BoneWeightData>> boneWeightData;
std::map<int, FbxAMatrix> toParents;
std::map<int, FbxAMatrix> locals;
std::map<int, FbxAMatrix> globals;
std::map<int, FbxNode*> nodes;

void ProcessHierarchy(FbxNode* node,
	std::vector<M3DLoader::SkinnedVertex>& vertices,
	std::vector<USHORT>& indices,
	SkinnedData& skinnedData,
	AnimationClip& clip,
	std::vector<XMFLOAT4X4>& boneOffsets,
	std::vector<int>& boneIndexToParentIndex,
	int parentIndex = -1)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	int boneIndex = boneIndexToParentIndex.size();

	if (attribute)
	{
		FbxNodeAttribute::EType attributeType = attribute->GetAttributeType();
		if (attributeType == FbxNodeAttribute::eMesh)
		{
			FbxMesh* mesh = node->GetMesh();

			int verticesCount = mesh->GetControlPointsCount();
			for (unsigned int i = 0; i < verticesCount; ++i)
			{
				M3DLoader::SkinnedVertex vertex;
				vertex.Pos.x = static_cast<float>(mesh->GetControlPointAt(i).mData[0]);
				vertex.Pos.y = static_cast<float>(mesh->GetControlPointAt(i).mData[1]);
				vertex.Pos.z = static_cast<float>(mesh->GetControlPointAt(i).mData[2]);

				vertices.push_back(vertex);
			}

			int polygonCount = mesh->GetPolygonCount();
			for (unsigned int i = 0; i < polygonCount; ++i)
			{
				for (unsigned int j = 0; j < 3; ++j)
				{
					int vertexIndex = mesh->GetPolygonVertex(i, j);
					indices.emplace_back(vertexIndex);

					if (mesh->GetElementNormalCount() < 1)
						continue;
					const FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(0);
					switch (vertexNormal->GetMappingMode())
					{
					case FbxGeometryElement::eByControlPoint:
						switch (vertexNormal->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
							vertices[vertexIndex].Normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexIndex).mData[0]);
							vertices[vertexIndex].Normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexIndex).mData[1]);
							vertices[vertexIndex].Normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexIndex).mData[2]);
							break;
						case FbxGeometryElement::eIndexToDirect:
							int index = vertexNormal->GetIndexArray().GetAt(vertexIndex);
							vertices[vertexIndex].Normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
							vertices[vertexIndex].Normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
							vertices[vertexIndex].Normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
							break;
						}
						break;
					case FbxGeometryElement::eByPolygonVertex:
						// ¹Ì±¸Çö
						break;
					}
				}
			}

			FbxVector4 T = node->GetGeometricTranslation(FbxNode::eSourcePivot);
			FbxVector4 R = node->GetGeometricRotation(FbxNode::eSourcePivot);
			FbxVector4 S = node->GetGeometricScaling(FbxNode::eSourcePivot);
			FbxAMatrix geometryTransform = FbxAMatrix(T, R, S);

			int deformerCount = mesh->GetDeformerCount();
			for (unsigned int i = 0; i < deformerCount; ++i)
			{
				FbxSkin* skin = reinterpret_cast<FbxSkin*>(mesh->GetDeformer(i, FbxDeformer::eSkin));
				if (!skin)
					continue;

				int clusterCount = skin->GetClusterCount();
				for (unsigned int j = 0; j < clusterCount; ++j)
				{
					FbxCluster* cluster = skin->GetCluster(j);
					std::string jointName = cluster->GetLink()->GetName();
					unsigned int jointIndex = skeletonIndexer[jointName];

					FbxAMatrix transform;
					FbxAMatrix linkTransform;

					cluster->GetTransformMatrix(transform);
					cluster->GetTransformLinkMatrix(linkTransform);

					FbxAMatrix global = linkTransform.Inverse() * transform;

					FbxAMatrix offset = global;
					
					FbxVector4 r1 = offset.GetRow(0);
					FbxVector4 r2 = offset.GetRow(1);
					FbxVector4 r3 = offset.GetRow(2);
					FbxVector4 r4 = offset.GetRow(3);
					
					boneOffsets[jointIndex] = XMFLOAT4X4{
						(float)r1.mData[0], (float)r1.mData[1], (float)r1.mData[2], (float)r1.mData[3],
						(float)r2.mData[0], (float)r2.mData[1], (float)r2.mData[2], (float)r2.mData[3],
						(float)r3.mData[0], (float)r3.mData[1], (float)r3.mData[2], (float)r3.mData[3],
						(float)r4.mData[0], (float)r4.mData[1], (float)r4.mData[2], (float)r4.mData[3]
					};

					int indicesCount = cluster->GetControlPointIndicesCount();
					for (int k = 0; k < indicesCount; ++k)
					{
						boneWeightData[cluster->GetControlPointIndices()[k]].push_back({ jointIndex, cluster->GetControlPointWeights()[k] });

						int vertexIndex = cluster->GetControlPointIndices()[k];
						float* boneWeight[3];
						boneWeight[0] = &vertices[vertexIndex].BoneWeights.x;
						boneWeight[1] = &vertices[vertexIndex].BoneWeights.y;
						boneWeight[2] = &vertices[vertexIndex].BoneWeights.z;
						for (int i = 0; i < 4; ++i)
						{
							if (i == 3);
							else if (*boneWeight[i])
								continue;
							if (i < 3)
								*boneWeight[i] = float(cluster->GetControlPointWeights()[k]);
							vertices[vertexIndex].BoneIndices[i] = jointIndex;
							break;
						}
					}
				}

				FbxArray<FbxString*> animStackNameArray;
				node->GetScene()->FillAnimStackNameArray(animStackNameArray);

				FbxAnimStack* animStack = node->GetScene()->FindMember<FbxAnimStack>(animStackNameArray[0]->Buffer());
				FbxAnimLayer* animLayer = animStack->GetMember<FbxAnimLayer>();
				node->GetScene()->SetCurrentAnimationStack(animStack);

				FbxString stackName = animStack->GetName();

				FbxTakeInfo* takeInfo = node->GetScene()->GetTakeInfo(*(animStackNameArray[0]));
				FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
				FbxTime end = takeInfo->mLocalTimeSpan.GetStop();

				for (unsigned int j = 0; j < clusterCount; ++j)
				{
					FbxCluster* cluster = skin->GetCluster(j);
					std::string jointName = cluster->GetLink()->GetName();
					unsigned int jointIndex = skeletonIndexer[jointName];

					BoneAnimation boneAnim;
					for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
					{
						FbxTime time;
						time.SetFrame(i, FbxTime::eFrames24);

						Keyframe keyframe;

						FbxAMatrix localTransform = cluster->GetLink()->EvaluateLocalTransform(time);
						FbxVector4 t = localTransform.GetT();
						FbxVector4 s = localTransform.GetS();
						FbxQuaternion q = localTransform.GetQ();

						keyframe.TimePos = i * 0.0166;
						keyframe.Translation = XMFLOAT3(t.mData[0], t.mData[1], t.mData[2]);
						keyframe.Scale = XMFLOAT3(s.mData[0], s.mData[1], s.mData[2]);
						keyframe.RotationQuat = XMFLOAT4(q.mData[0], q.mData[1], q.mData[2], q.mData[3]);

						boneAnim.Keyframes.push_back(keyframe);
					}
					clip.BoneAnimations[jointIndex] = boneAnim;
				}

				for (int j = 0; j < 80; ++j)
				{
					BoneAnimation boneAnim;
					if (clip.BoneAnimations[j].Keyframes.size() == 0)
					{

						for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
						{
							FbxTime time;
							time.SetFrame(i, FbxTime::eFrames24);

							Keyframe keyframe;

							FbxAMatrix localTransform = nodes[j]->EvaluateLocalTransform(time);

							FbxVector4 t = localTransform.GetT();
							FbxVector4 s = localTransform.GetS();
							FbxQuaternion q = localTransform.GetQ();

							keyframe.TimePos = i;
							keyframe.Translation = XMFLOAT3(t.mData[0], t.mData[1], t.mData[2]);
							keyframe.Scale = XMFLOAT3(s.mData[0], s.mData[1], s.mData[2]);
							keyframe.RotationQuat = XMFLOAT4(q.mData[0], q.mData[1], q.mData[2], q.mData[3]);

							boneAnim.Keyframes.push_back(keyframe);
						}
						clip.BoneAnimations[j] = boneAnim;
					}
				}
			}
		}
		else if (attributeType == FbxNodeAttribute::eSkeleton)
		{
			skeletonIndexer[node->GetName()] = boneIndex;
			boneIndexToParentIndex.push_back(parentIndex);

			nodes[boneIndex] = node;
		}
	}

	if (boneIndexToParentIndex.size() == 0)
		boneIndex = -1;

	int childCount = node->GetChildCount();
	for (int i = 0; i < childCount; ++i)
	{
		ProcessHierarchy(node->GetChild(i), vertices, indices, skinnedData, clip, boneOffsets, boneIndexToParentIndex, boneIndex);
	}
}

RendererManager::RendererManager()
{
}

RendererManager::~RendererManager()
{
}

void RendererManager::Initialize()
{
	InitDirect3D();
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

	SkinnedModelInstance* skinnedModelInst{ nullptr };
	// UpdateObjectCBs
	auto currObjectCB = currFrameResource->ObjectCB.get();
	for (auto& e : allRItems)
	{
		if (e->NumFramesDirty > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&e->World);
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
	for (auto& e : materials)
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

void RendererManager::Start()
{
	commandList->Reset(commandAllocator.Get(), nullptr);
	LoadAssets();
}

void RendererManager::Update()
{
}

void RendererManager::PreRender()
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

void RendererManager::Render()
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
	for (auto& renderItems : renderItemLayer)
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

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();

	swapChain->SetFullscreenState(FALSE, NULL);
}

void RendererManager::InitDirect3D()
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

void RendererManager::LoadAssets()
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

	std::vector<M3DLoader::Subset> mSkinnedSubsets;
	std::vector<M3DLoader::M3dMaterial> mSkinnedMats;
	SkinnedModelInstance* mSkinnedModelInst;
	{
		SkinnedData* mSkinnedInfo = new SkinnedData();

		std::vector<M3DLoader::SkinnedVertex> vertices;
		std::vector<std::uint16_t> indices;

		M3DLoader m3dLoader;
		m3dLoader.LoadM3d("..\\CyanEngine\\Models\\soldier.m3d", vertices, indices,
			mSkinnedSubsets, mSkinnedMats, *mSkinnedInfo);

		{
			FbxManager* manager = FbxManager::Create();
		
			FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
			manager->SetIOSettings(ios);
		
			FbxScene* scene = FbxScene::Create(manager, "Scene");
		
			FbxImporter* importer = FbxImporter::Create(manager, "");
			importer->Initialize("..\\CyanEngine\\Models\\humanoid.fbx");
			importer->Import(scene);
		
			{
				FbxGeometryConverter geometryConverter(manager);
				geometryConverter.Triangulate(scene, true);
		
				std::vector<XMFLOAT4X4> boneOffsets;
				std::vector<int> boneIndexToParentIndex;
				std::unordered_map<std::string, AnimationClip> animations;
				AnimationClip clip;
		
				vertices.clear();
				indices.clear();
				boneOffsets.resize(80);
				clip.BoneAnimations.resize(80);
				for (auto& offset : boneOffsets)
					offset = MathHelper::Identity4x4();

		
				ProcessHierarchy(scene->GetRootNode(), vertices, indices, *mSkinnedInfo, clip, boneOffsets, boneIndexToParentIndex);

				for (auto iter = clip.BoneAnimations.begin(); iter != clip.BoneAnimations.end(); ++iter)
				{
					if (iter->Keyframes.size() == 0)
					{
						for (int i = 0; i < 25; ++i)
						{
							Keyframe keyframe;

							FbxAMatrix matrix;
							matrix.SetIdentity();

							FbxVector4 t = matrix.GetT();
							FbxVector4 s = matrix.GetS();
							FbxQuaternion q = matrix.GetQ();

							keyframe.TimePos = i;
							keyframe.Translation = XMFLOAT3(t.mData[0], t.mData[1], t.mData[2]);
							keyframe.Scale = XMFLOAT3(s.mData[0], s.mData[1], s.mData[2]);
							keyframe.RotationQuat = XMFLOAT4(q.mData[0], q.mData[1], q.mData[2], q.mData[3]);
							//keyframe.Translation = XMFLOAT3(0, 0, 0);
							//keyframe.Scale = XMFLOAT3(1, 1, 1);
							//keyframe.RotationQuat = XMFLOAT4(0, 0, 0, 1);

							iter->Keyframes.push_back(keyframe);
						}
					}
				}
				animations["run"] = clip;
		
				delete mSkinnedInfo;
				mSkinnedInfo = new SkinnedData();
				mSkinnedInfo->Set(boneIndexToParentIndex, boneOffsets, animations);
			}
		
			if (manager)
				manager->Destroy();
		}
		mSkinnedSubsets.resize(1);

		for (UINT i = 0; i < 1; ++i)
		{
			mSkinnedSubsets[i].Id = 0;
			mSkinnedSubsets[i].VertexStart = 0;
			mSkinnedSubsets[i].VertexCount = vertices.size();
			mSkinnedSubsets[i].FaceStart = 0;
			mSkinnedSubsets[i].FaceCount = indices.size();
		}

		mSkinnedModelInst = new SkinnedModelInstance();
		mSkinnedModelInst->SkinnedInfo = mSkinnedInfo;
		mSkinnedModelInst->FinalTransforms.resize(mSkinnedInfo->BoneCount());
		//mSkinnedModelInst->ClipName = "Take1";
		mSkinnedModelInst->ClipName = "run";
		mSkinnedModelInst->TimePos = 0.0f;

		const UINT vbByteSize = (UINT)vertices.size() * sizeof(FrameResource::SkinnedVertex);
		const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

		auto geo = std::make_unique<MeshGeometry>();
		geo->Name = mSkinnedModelFilename;

		ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
		CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

		ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
		CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);
		geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(), commandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

		geo->VertexByteStride = sizeof(FrameResource::SkinnedVertex);
		geo->VertexBufferByteSize = vbByteSize;
		geo->IndexFormat = DXGI_FORMAT_R16_UINT;
		geo->IndexBufferByteSize = ibByteSize;

		for (UINT i = 0; i < (UINT)mSkinnedSubsets.size(); ++i)
		{
			SubmeshGeometry submesh;
			std::string name = "sm_" + std::to_string(i);

			submesh.IndexCount = (UINT)mSkinnedSubsets[i].FaceCount * 3;
			submesh.StartIndexLocation = mSkinnedSubsets[i].FaceStart * 3;
			submesh.BaseVertexLocation = 0;

			geo->DrawArgs[name] = submesh;
		}

		geometries[geo->Name] = std::move(geo);


		UINT matCBIndex = 4;
		UINT srvHeapIndex = 5;// mSkinnedSrvHeapStart;
		for (UINT i = 0; i < mSkinnedMats.size(); ++i)
		{
			auto mat = std::make_unique<Material>();
			mat->Name = mSkinnedMats[i].Name;
			mat->MatCBIndex = matCBIndex++;
			mat->DiffuseSrvHeapIndex = srvHeapIndex++;
			mat->NormalSrvHeapIndex = srvHeapIndex;// srvHeapIndex++;
			mat->DiffuseAlbedo = mSkinnedMats[i].DiffuseAlbedo;
			mat->FresnelR0 = mSkinnedMats[i].FresnelR0;
			mat->Roughness = mSkinnedMats[i].Roughness;

			materials[mat->Name] = std::move(mat);
		}
	}
	for (int i = 0; i < mSkinnedMats.size(); ++i)
	{
		std::string diffuseName = mSkinnedMats[i].DiffuseMapName;
		std::wstring diffuseFilename = L"..\\CyanEngine\\Textures\\" + AnsiToWString(diffuseName);
	
		diffuseName = diffuseName.substr(0, diffuseName.find_last_of("."));
	
		textureData.push_back({ diffuseName, diffuseFilename });
	}

	std::vector<std::string> texName;
	for (auto& d : textureData)
	{
		if (textures.find(d.name) == std::end(textures))
		{
			auto texture = std::make_unique<Texture>();
			texture->Name = d.name;
			texture->Filename = d.fileName;

			texName.push_back(d.name);

			CreateDDSTextureFromFile12(device.Get(), commandList.Get(), texture->Filename.c_str(), texture->Resource, texture->UploadHeap);
			textures[texture->Name] = std::move(texture);
		}
	}
	{
		UINT objCBIndex = allRItems.size();

		int count = 0;
		float interval = 2.5f;
		for(int x = -count; x <= count; ++x)
			for(int z = -count; z <= count; ++z)
				for (UINT i = 0; i < mSkinnedMats.size(); ++i)
				{
					std::string submeshName = "sm_" + std::to_string(i);

					auto ritem = std::make_unique<RenderItem>();

					XMMATRIX modelScale = XMMatrixScaling(0.005, 0.005, 0.005);
					XMMATRIX modelRot = XMMatrixRotationX(-PI * 0.5);
					//XMMATRIX modelScale = XMMatrixScaling(0.2, 0.2, 0.2);
					//XMMATRIX modelRot = XMMatrixRotationX(0);
					XMMATRIX modelOffset = XMMatrixTranslation(0, 0, 0);
					XMStoreFloat4x4(&ritem->World, modelScale * modelRot * modelOffset);

					ritem->TexTransform = MathHelper::Identity4x4();
					ritem->ObjCBIndex = objCBIndex++;
					ritem->Mat = materials[mSkinnedMats[i].Name].get();
					ritem->Geo = geometries[mSkinnedModelFilename].get();
					ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
					ritem->IndexCount = ritem->Geo->DrawArgs[submeshName].IndexCount;
					ritem->StartIndexLocation = ritem->Geo->DrawArgs[submeshName].StartIndexLocation;
					ritem->BaseVertexLocation = ritem->Geo->DrawArgs[submeshName].BaseVertexLocation;

					ritem->SkinnedCBIndex = 0;
					ritem->SkinnedModelInst = mSkinnedModelInst;

					renderItemLayer[(int)RenderLayer::SkinnedOpaque].push_back(ritem.get());
					allRItems.push_back(std::move(ritem));
				}
	}


	commandList->Close();
	ID3D12CommandList* cmdsLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	
	for (int i = 0; i < NumFrameResources; ++i)
		frameResources.push_back(std::make_unique<FrameResource>(device.Get(), 1, (UINT)allRItems.size(), 1, (UINT)materials.size()));


	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.NumDescriptors = 9;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&srvHeap));

	{
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(srvHeap->GetCPUDescriptorHandleForHeapStart());

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		for (auto& d : texName)
		{
			srvDesc.Format = textures[d]->Resource->GetDesc().Format;
			srvDesc.Texture2D.MipLevels = textures[d]->Resource->GetDesc().MipLevels;
			device->CreateShaderResourceView(textures[d]->Resource.Get(), &srvDesc, handle);

			handle.Offset(1, srvDescriptorSize);
		}
	}

	WaitForPreviousFrame();

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
		HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		fence->SetEventOnCompletion(_fence, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
		CloseHandle(fenceEvent);
	}

	frameIndex = swapChain->GetCurrentBackBufferIndex();
}
