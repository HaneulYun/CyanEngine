#include "pch.h"
#include "Scene.h"

struct BoneWeightData
{
	unsigned int boneIndex;
	double weight;
};

std::map<std::string, int> skeletonIndexer;
std::map<int, std::vector<BoneWeightData>> boneWeightData;
std::map<int, FbxNode*> nodes;
std::vector<M3DLoader::Subset> subsets;

void LoadModel(FbxNode* node,
	std::vector<M3DLoader::SkinnedVertex>& vertices,
	std::vector<USHORT>& indices,
	std::vector<XMFLOAT4X4>& boneOffsets,
	std::vector<int>& boneIndexToParentIndex, FbxGeometryConverter geometryConverter,
	int parentIndex = -1)
{
	static int kk = 0;
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	int boneIndex = boneIndexToParentIndex.size();

	if (attribute)
	{
		FbxNodeAttribute::EType attributeType = attribute->GetAttributeType();
		if (attributeType == FbxNodeAttribute::eMesh)
		{
			if (!kk)
			{
				kk = 1;
				geometryConverter.SplitMeshPerMaterial(node->GetMesh(), false);
			}
			else
			{
				FbxMesh* mesh = node->GetMesh();
				int materialCount = node->GetSrcObjectCount<FbxSurfaceMaterial>();
				if (materialCount > 0)
				{
					for (int k = 0; k < materialCount; ++k)
					{
						FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)node->GetSrcObject<FbxSurfaceMaterial>(k);
						const char* name = material->GetName();
						if (material != NULL)
						{
							FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

							int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();
							if (layeredTextureCount > 0)
							{
								for (int i = 0; i < layeredTextureCount; ++i)
								{
									FbxLayeredTexture* layeredTexture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(i));
									int textureCount = layeredTexture->GetSrcObjectCount<FbxTexture>();
									for (int j = 0; j < textureCount; ++j)
									{
										FbxTexture* texture = FbxCast<FbxTexture>(layeredTexture->GetSrcObject<FbxTexture>(j));
										const char* textrueName = texture->GetName();
										//texture->UVSet;
									}
								}
							}
						}
					}
				}

				M3DLoader::Subset s;
				s.VertexStart = vertices.size();

				int verticesCount = mesh->GetControlPointsCount();
				for (unsigned int i = 0; i < verticesCount; ++i)
				{
					M3DLoader::SkinnedVertex vertex;
					vertex.Pos.x = static_cast<float>(mesh->GetControlPointAt(i).mData[0]);
					vertex.Pos.y = static_cast<float>(mesh->GetControlPointAt(i).mData[1]);
					vertex.Pos.z = static_cast<float>(mesh->GetControlPointAt(i).mData[2]);

					vertices.push_back(vertex);
				}

				s.VertexCount = vertices.size();				
				s.FaceStart = indices.size();

				// normal, Texcoord, Tangent ·Îµå
				int polygonCount = mesh->GetPolygonCount();
				for (unsigned int i = 0; i < polygonCount; ++i)
				{
					// normal
					for (unsigned int j = 0; j < 3; ++j)
					{
						int vertexIndex = mesh->GetPolygonVertex(i, j);
						//int vertexIndex = mesh->GetPolygonVertex(i, j) + s.VertexStart;
						indices.emplace_back(vertexIndex + s.VertexStart);

						if (mesh->GetElementNormalCount() < 1)
							continue;
						const FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(0);
						switch (vertexNormal->GetMappingMode())
						{
						case FbxGeometryElement::eByControlPoint:
							switch (vertexNormal->GetReferenceMode())
							{
							case FbxGeometryElement::eDirect:
								vertices[vertexIndex + s.VertexStart].Normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexIndex).mData[0]);
								vertices[vertexIndex + s.VertexStart].Normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexIndex).mData[1]);
								vertices[vertexIndex + s.VertexStart].Normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexIndex).mData[2]);
								break;
							case FbxGeometryElement::eIndexToDirect:
								int index = vertexNormal->GetIndexArray().GetAt(vertexIndex);
								vertices[vertexIndex + s.VertexStart].Normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
								vertices[vertexIndex + s.VertexStart].Normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
								vertices[vertexIndex + s.VertexStart].Normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
								break;
							}
							break;
						}
					}

					// uvs
					for (unsigned int j = 0; j < 3; ++j)
					{
						int vertexIndex = mesh->GetPolygonVertex(i, j);
						//int vertexIndex = mesh->GetPolygonVertex(i, j) + s.VertexStart;
						//indices.emplace_back(vertexIndex);
						if (mesh->GetElementUVCount() < 1)
							continue;
						const FbxGeometryElementUV* vertexUv = mesh->GetElementUV(0);
						switch (vertexUv->GetMappingMode())
						{
						case FbxGeometryElement::eByControlPoint:
							switch (vertexUv->GetReferenceMode())
							{
							case FbxGeometryElement::eDirect:
								vertices[vertexIndex + s.VertexStart].TexC.x = static_cast<float>(vertexUv->GetDirectArray().GetAt(vertexIndex).mData[0]);
								vertices[vertexIndex + s.VertexStart].TexC.y = static_cast<float>(vertexUv->GetDirectArray().GetAt(vertexIndex).mData[1]);
								break;
							case FbxGeometryElement::eIndexToDirect:
								int index = vertexUv->GetIndexArray().GetAt(vertexIndex);
								vertices[vertexIndex + s.VertexStart].TexC.x = static_cast<float>(vertexUv->GetDirectArray().GetAt(index).mData[0]);
								vertices[vertexIndex + s.VertexStart].TexC.y = static_cast<float>(vertexUv->GetDirectArray().GetAt(index).mData[1]);
								break;
							}
							break;
						}

					}


					// materialIndices
					/*for (unsigned int j = 0; j < 3; ++j)
					{
						int vertexIndex = mesh->GetPolygonVertex(i, j);
						FbxSurfaceMaterial* material = node->GetMaterial(vertexIndex);

						for (int k = 0; k < node->GetSrcObjectCount<FbxSurfaceMaterial>(); ++k)
						{
							FbxSurfaceMaterial* m = node->GetSrcObject<FbxSurfaceMaterial>(k);
							if (m == material)
								vertices[vertexIndex].MaterialIndex = k;
						}
					}*/
					
				}

				s.FaceCount = indices.size() - s.FaceStart;
				subsets.push_back(s);

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

						if (boneOffsets.size() <= jointIndex)
							boneOffsets.resize(jointIndex + 1);
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

							int vertexIndex = cluster->GetControlPointIndices()[k] + s.VertexStart;
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
				}
			}
		}
		else if (attributeType == FbxNodeAttribute::eSkeleton)
		{
			skeletonIndexer[node->GetName()] = boneIndex;
			boneIndexToParentIndex.push_back(parentIndex);
		}
	}

	if (boneIndexToParentIndex.size() == 0)
		boneIndex = -1;

	int childCount = node->GetChildCount();
	for (int i = 0; i < childCount; ++i)
	{
		LoadModel(node->GetChild(i), vertices, indices, boneOffsets, boneIndexToParentIndex, geometryConverter, boneIndex);
	}
}

void LoadAnimation(FbxNode* node,
	AnimationClip& clip,
	std::vector<int>& boneIndexToParentIndex,
	int parentIndex = -2)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	int boneIndex = boneIndexToParentIndex.size();

	if (attribute)
	{
		FbxNodeAttribute::EType attributeType = attribute->GetAttributeType();
		if (attributeType == FbxNodeAttribute::eSkeleton)
		{
			boneIndexToParentIndex.push_back(parentIndex);

			nodes[boneIndex] = node;
		}
	}

	if (boneIndexToParentIndex.size() == 0)
		boneIndex = -1;

	int childCount = node->GetChildCount();
	for (int i = 0; i < childCount; ++i)
	{
		LoadAnimation(node->GetChild(i), clip, boneIndexToParentIndex, boneIndex);
	}
	if (parentIndex == -2)
	{
		FbxArray<FbxString*> animStackNameArray;
		node->GetScene()->FillAnimStackNameArray(animStackNameArray);

		FbxAnimStack* animStack = node->GetScene()->FindMember<FbxAnimStack>(animStackNameArray[0]->Buffer());
		node->GetScene()->SetCurrentAnimationStack(animStack);

		FbxAnimLayer* animLayer = animStack->GetMember<FbxAnimLayer>();
		FbxTakeInfo* takeInfo = node->GetScene()->GetTakeInfo(*(animStackNameArray[0]));
		FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
		FbxTime end = takeInfo->mLocalTimeSpan.GetStop();

		for (unsigned int j = 0; j < clip.BoneAnimations.size(); ++j)
		{
			std::string str = nodes[j]->GetName();
			if (nodes[j]->GetName() == "hand_r")
				int k = 0;

			BoneAnimation boneAnim;
			for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
			{

				FbxTime time;
				time.SetFrame(i, FbxTime::eFrames24);

				Keyframe keyframe;

				FbxAMatrix localTransform = nodes[j]->EvaluateLocalTransform(time);
				FbxVector4 t = localTransform.GetT();
				FbxVector4 s = localTransform.GetS();
				FbxQuaternion q = localTransform.GetQ();

				keyframe.TimePos = i * 0.166;
				keyframe.Translation = XMFLOAT3(t.mData[0], t.mData[1], t.mData[2]);
				keyframe.Scale = XMFLOAT3(s.mData[0], s.mData[1], s.mData[2]);
				keyframe.RotationQuat = XMFLOAT4(q.mData[0], q.mData[1], q.mData[2], q.mData[3]);

				boneAnim.Keyframes.push_back(keyframe);
			}
			clip.BoneAnimations[j] = boneAnim;
		}

		for (int j = 0; j < clip.BoneAnimations.size(); ++j)
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

					keyframe.TimePos = i * 0.166;
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

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Start()
{
	BuildObjects();

	Graphics::Instance()->commandList->Reset(Graphics::Instance()->commandAllocator.Get(), nullptr);

	std::string mSkinnedModelFilename = "Models\\soldier.m3d";

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
			importer->Initialize("..\\CyanEngine\\Models\\modelTest2.fbx");
			importer->Import(scene);
		
			FbxScene* scene2 = FbxScene::Create(manager, "Scene2");
		
			FbxImporter* importer2 = FbxImporter::Create(manager, "");
			importer2->Initialize("..\\CyanEngine\\Models\\animTest2.fbx");
			importer2->Import(scene2);
		
			{
				FbxGeometryConverter geometryConverter(manager);
				geometryConverter.Triangulate(scene, true);
		
				std::vector<XMFLOAT4X4> boneOffsets;
				std::vector<int> boneIndexToParentIndex;
				std::unordered_map<std::string, AnimationClip> animations;
				AnimationClip clip;
		
				vertices.clear();
				indices.clear();
				for (auto& offset : boneOffsets)
					offset = MathHelper::Identity4x4();
		
				LoadModel(scene->GetRootNode(), vertices, indices, boneOffsets, boneIndexToParentIndex, geometryConverter);
				boneIndexToParentIndex.clear();
				clip.BoneAnimations.resize(boneOffsets.size());
				LoadAnimation(scene2->GetRootNode(), clip, boneIndexToParentIndex);
		
				animations["run"] = clip;
		
				delete mSkinnedInfo;
				mSkinnedInfo = new SkinnedData();
				mSkinnedInfo->Set(boneIndexToParentIndex, boneOffsets, animations);
			}
		
			if (manager)
				manager->Destroy();
		}
		/*mSkinnedSubsets.resize(1);

		for (UINT i = 0; i < 1; ++i)
		{
			mSkinnedSubsets[i].Id = 0;
			mSkinnedSubsets[i].VertexStart = 0;
			mSkinnedSubsets[i].VertexCount = vertices.size();
			mSkinnedSubsets[i].FaceStart = 0;
			mSkinnedSubsets[i].FaceCount = indices.size();
		}*/
		mSkinnedSubsets.resize(subsets.size());

		for (UINT i = 0; i < subsets.size(); ++i)
		{
			mSkinnedSubsets[i].Id = i;
			mSkinnedSubsets[i].VertexStart = subsets[i].VertexStart;
			mSkinnedSubsets[i].VertexCount = subsets[i].VertexCount;
			mSkinnedSubsets[i].FaceStart = subsets[i].FaceStart;
			mSkinnedSubsets[i].FaceCount = subsets[i].FaceCount;
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

		geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(
			Graphics::Instance()->device.Get(), Graphics::Instance()->commandList.Get(),
			vertices.data(), vbByteSize, geo->VertexBufferUploader);
		geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(
			Graphics::Instance()->device.Get(), Graphics::Instance()->commandList.Get(),
			indices.data(), ibByteSize, geo->IndexBufferUploader);

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

		Scene::scene->geometries[geo->Name] = std::move(geo);


		UINT matCBIndex = 4;
		UINT srvHeapIndex = 0;// mSkinnedSrvHeapStart;
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

			Scene::scene->materials[mat->Name] = std::move(mat);
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
		if (Scene::scene->textures.find(d.name) == std::end(Scene::scene->textures))
		{
			auto texture = std::make_unique<Texture>();
			texture->Name = d.name;
			texture->Filename = d.fileName;

			texName.push_back(d.name);

			CreateDDSTextureFromFile12(
				Graphics::Instance()->device.Get(), Graphics::Instance()->commandList.Get(),
				texture->Filename.c_str(), texture->Resource, texture->UploadHeap);
			Scene::scene->textures[texture->Name] = std::move(texture);
		}
	}
	{
		UINT objCBIndex = Scene::scene->allRItems.size();

		int count = 0;
		float interval = 2.5f;
		for (int x = -count; x <= count; ++x)
			for (int z = -count; z <= count; ++z)
				for (UINT i = 0; i < mSkinnedMats.size(); ++i)
				{
					std::string submeshName = "sm_" + std::to_string(i);

					auto ritem = std::make_unique<RenderItem>();

					XMMATRIX modelScale = XMMatrixScaling(0.03, 0.03, 0.03);
					XMMATRIX modelRot = XMMatrixRotationX(-PI * 0.5);
					//XMMATRIX modelScale = XMMatrixScaling(0.05, 0.05, -0.05);
					//XMMATRIX modelRot = XMMatrixRotationX(0);
					XMMATRIX modelOffset = XMMatrixTranslation(x * 3, 0, z * 3);
					XMStoreFloat4x4(&ritem->World, modelScale * modelRot * modelOffset);

					ritem->TexTransform = MathHelper::Identity4x4();
					ritem->ObjCBIndex = objCBIndex++;
					ritem->Mat = Scene::scene->materials[mSkinnedMats[i].Name].get();
					ritem->Geo = Scene::scene->geometries[mSkinnedModelFilename].get();
					ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
					ritem->IndexCount = ritem->Geo->DrawArgs[submeshName].IndexCount;
					ritem->StartIndexLocation = ritem->Geo->DrawArgs[submeshName].StartIndexLocation;
					ritem->BaseVertexLocation = ritem->Geo->DrawArgs[submeshName].BaseVertexLocation;

					ritem->SkinnedCBIndex = 0;
					ritem->SkinnedModelInst = mSkinnedModelInst;

					Scene::scene->renderItemLayer[(int)RenderLayer::SkinnedOpaque].push_back(ritem.get());
					Scene::scene->allRItems.push_back(std::move(ritem));
				}
	}

	Graphics::Instance()->commandList->Close();
	ID3D12CommandList* cmdsLists[] = { Graphics::Instance()->commandList.Get() };
	Graphics::Instance()->commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	{
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(Graphics::Instance()->srvHeap->GetCPUDescriptorHandleForHeapStart());

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		for (auto& d : texName)
		{
			srvDesc.Format = Scene::scene->textures[d]->Resource->GetDesc().Format;
			srvDesc.Texture2D.MipLevels = Scene::scene->textures[d]->Resource->GetDesc().MipLevels;
			Graphics::Instance()->device->CreateShaderResourceView(Scene::scene->textures[d]->Resource.Get(), &srvDesc, handle);

			handle.Offset(1, Graphics::Instance()->srvDescriptorSize);
		}
	}

	for (int i = 0; i < NUM_FRAME_RESOURCES; ++i)
		frameResources.push_back(std::make_unique<FrameResource>(
			Graphics::Instance()->device.Get(), 1, (UINT)Scene::scene->allRItems.size(), 1, (UINT)5));
}

void Scene::Update()
{
	if (isDirty)
	{
		isDirty = false;
		Start();
	}

	// fixed update
	Collider *lhs_collider, *rhs_collider;
	for (auto lhs_iter = gameObjects.begin(); lhs_iter != gameObjects.end(); ++lhs_iter)
		if (lhs_collider = (*lhs_iter)->GetComponent<Collider>())
			for (auto rhs_iter = gameObjects.begin(); rhs_iter != gameObjects.end(); ++rhs_iter)
				if(lhs_iter != rhs_iter)
					if (rhs_collider = (*rhs_iter)->GetComponent<Collider>())
					{
						auto iter = (*lhs_iter)->collisionType.find(*rhs_iter);
	
						if (lhs_collider->Compare(rhs_collider))
						{
							if (iter == (*lhs_iter)->collisionType.end())
								((*lhs_iter)->collisionType)[*rhs_iter] = CollisionType::eTriggerEnter;
							else if (iter->second == CollisionType::eTriggerEnter)
								iter->second = CollisionType::eTriggerStay;
						}
						else if (iter != (*lhs_iter)->collisionType.end())
							iter->second = CollisionType::eTriggerExit;
					}

	for (GameObject* gameObject : gameObjects)
	{
		for (auto d : gameObject->collisionType)
		{
			switch (d.second)
			{
			case CollisionType::eCollisionEnter:
				gameObject->OnCollisionEnter(d.first); break;
			case CollisionType::eCollisionStay:
				gameObject->OnCollisionStay(d.first); break;
			case CollisionType::eCollisionExit:
				gameObject->OnCollisionExit(d.first); break;
			case CollisionType::eTriggerEnter:
				gameObject->OnTriggerEnter(d.first); break;
			case CollisionType::eTriggerStay:
				gameObject->OnTriggerStay(d.first); break;
			case CollisionType::eTriggerExit:
				gameObject->OnTriggerExit(d.first); break;
			}
		}
	}

	// update
	for (GameObject* gameObject : gameObjects)
		gameObject->Update();

	while (!deletionQueue.empty())
	{
		GameObject* gameObject = deletionQueue.top();
		Delete(gameObject);
		deletionQueue.pop();
	}
}

void Scene::BuildObjects()
{
}

void Scene::ReleaseObjects()
{
	for (GameObject* object : gameObjects)
		delete object;
	gameObjects.clear();
}


GameObject* Scene::AddGameObject(GameObject* gameObject)
{
	gameObject->scene = this;
	gameObjects.push_back(gameObject);
	return gameObject;
}

GameObject* Scene::CreateEmpty(bool addition)
{
	return new GameObject(addition);
}

GameObject* Scene::Duplicate(GameObject* gameObject)
{
	return new GameObject(gameObject);
}

GameObject* Scene::CreateEmptyPrefab()
{
	return new GameObject(false);
}

GameObject* Scene::DuplicatePrefab(GameObject* gameObject)
{
	return new GameObject(gameObject, false);
}

void Scene::PushDelete(GameObject* gameObject)
{
	deletionQueue.push(gameObject);
}

void Scene::Delete(GameObject* gameObject)
{
	for (auto iter = gameObjects.begin(); iter != gameObjects.end(); ++iter)
		if ((*iter)->collisionType.find(gameObject) != (*iter)->collisionType.end())
			(*iter)->collisionType.erase(gameObject);
	for (auto iter = gameObjects.begin(); iter != gameObjects.end(); ++iter)
		if (*iter == gameObject)
		{
			delete (*iter);
			gameObjects.erase(iter);
			return;
		}
}