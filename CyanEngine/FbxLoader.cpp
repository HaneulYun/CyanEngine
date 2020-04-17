#include "pch.h"
#include "FbxLoader.h"

void FbxModelData::LoadFbx(const char* path)
{
	FbxManager* manager = FbxManager::Create();

	FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ios);

	FbxScene* scene = FbxScene::Create(manager, "Scene");

	FbxImporter* importer = FbxImporter::Create(manager, "");
	importer->Initialize(path);
	importer->Import(scene);

	LoadFbxHierarchy(scene->GetRootNode());

	FbxArray<FbxString*> animStackNameArray;
	scene->FillAnimStackNameArray(animStackNameArray);


	int stackCount = animStackNameArray.GetCount();
	for (int stackIndex = 0; stackIndex < stackCount; ++stackIndex)
	{
		auto clip = std::make_unique<AnimationClip>();
		clip->BoneAnimations.resize(skeletonIndexer.size());

		FbxAnimStack* animStack = scene->FindMember<FbxAnimStack>(animStackNameArray[stackIndex]->Buffer());

		FbxAnimCurveFilterConstantKeyReducer* reducer = new FbxAnimCurveFilterConstantKeyReducer();
		reducer->SetTranslationThreshold(0.5f);
		reducer->SetRotationThreshold(0.5f);
		reducer->SetScalingThreshold(0.5f);
		reducer->SetDefaultThreshold(0.5f);
		reducer->SetKeepFirstAndLastKeys(true);
		reducer->Apply(animStack);

		scene->SetCurrentAnimationStack(animStack);

		FbxAnimLayer* animLayer = animStack->GetMember<FbxAnimLayer>();

		for (unsigned int i = 0; i < clip->BoneAnimations.size(); ++i)
		{
			BoneAnimation boneAnim;

			std::vector<std::pair<int,FbxAnimCurve*>> curves;
			FbxAnimCurve* curve;

			curve = nodes[i]->LclTranslation.GetCurve(animLayer, "X"); if(curve) curves.push_back(std::make_pair(curve->KeyGetCount(), curve));
			curve = nodes[i]->LclTranslation.GetCurve(animLayer, "Y"); if(curve) curves.push_back(std::make_pair(curve->KeyGetCount(), curve));
			curve = nodes[i]->LclTranslation.GetCurve(animLayer, "Z"); if(curve) curves.push_back(std::make_pair(curve->KeyGetCount(), curve));
			curve = nodes[i]->LclScaling.GetCurve(animLayer, "X"); if(curve) curves.push_back(std::make_pair(curve->KeyGetCount(), curve));
			curve = nodes[i]->LclScaling.GetCurve(animLayer, "Y"); if(curve) curves.push_back(std::make_pair(curve->KeyGetCount(), curve));
			curve = nodes[i]->LclScaling.GetCurve(animLayer, "Z"); if(curve) curves.push_back(std::make_pair(curve->KeyGetCount(), curve));
			curve = nodes[i]->LclRotation.GetCurve(animLayer, "X"); if(curve) curves.push_back(std::make_pair(curve->KeyGetCount(), curve));
			curve = nodes[i]->LclRotation.GetCurve(animLayer, "Y"); if(curve) curves.push_back(std::make_pair(curve->KeyGetCount(), curve));
			curve = nodes[i]->LclRotation.GetCurve(animLayer, "Z"); if(curve) curves.push_back(std::make_pair(curve->KeyGetCount(), curve));

			if (!curves.size()) continue;

			auto max = std::max_element(curves.begin(), curves.end());
			int curveNum = std::distance(curves.begin(), max);
			int maxCount = max->first;
			for (int j = 0; j < maxCount; ++j)
			{
				FbxTime fbxKeyTime = curves[curveNum].second->KeyGetTime(j);
				float keyTime = (float)fbxKeyTime.GetSecondDouble();

				Keyframe keyframe;
				FbxAMatrix localTransform = nodes[i]->EvaluateLocalTransform(fbxKeyTime);
				FbxVector4 t = localTransform.GetT();
				FbxVector4 s = localTransform.GetS();
				FbxQuaternion q = localTransform.GetQ();

				keyframe.TimePos = keyTime;
				keyframe.Translation = XMFLOAT3(t.mData[0], t.mData[1], t.mData[2]);
				keyframe.Scale = XMFLOAT3(s.mData[0], s.mData[1], s.mData[2]);
				keyframe.RotationQuat = XMFLOAT4(q.mData[0], q.mData[1], q.mData[2], q.mData[3]);

				boneAnim.Keyframes.push_back(keyframe);
			}
			clip->BoneAnimations[i] = boneAnim;
		}
		

		Scene::scene->animationClips["k"] = std::move(clip);
	}
}

void FbxModelData::LoadFbxHierarchy(FbxNode* node)
{
	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
	if (nodeAttribute)
	{
		FbxNodeAttribute::EType attributeType = nodeAttribute->GetAttributeType();
		if (attributeType == FbxNodeAttribute::eSkeleton)
			LoadFbxHierarchyRecursive(node);
		else if (attributeType == FbxNodeAttribute::eMesh)
			LoadFbxMesh(node);
		else if (attributeType == FbxNodeAttribute::eLODGroup)
		{
			const int childCount = node->GetChildCount();
			for (int childIndex = 0; childIndex < childCount; ++childIndex)
				LoadFbxHierarchy(node->GetChild(childIndex));
		}
	}
	else
	{
		const int childCount = node->GetChildCount();
		for (int childIndex = 0; childIndex < childCount; ++childIndex)
			LoadFbxHierarchy(node->GetChild(childIndex));
	}
}

void FbxModelData::LoadFbxHierarchyRecursive(FbxNode* node, int parentIndex)
{
	int boneIndex = skeletonIndexer.size();
	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
	if (nodeAttribute)
	{
		FbxNodeAttribute::EType attributeType = nodeAttribute->GetAttributeType();
		if (attributeType == FbxNodeAttribute::eSkeleton)
		{
			parentIndexer.push_back(parentIndex);
			skeletonIndexer[node->GetName()] = boneIndex;
			nodes[boneIndex] = node;

			const int childCount = node->GetChildCount();
			for (int childIndex = 0; childIndex < childCount; ++childIndex)
				LoadFbxHierarchyRecursive(node->GetChild(childIndex), boneIndex);
		}
	}
}

void FbxModelData::LoadFbxMesh(FbxNode* node)
{
	if (loadedMesh)
		return;
	loadedMesh = true;

	std::vector<FrameResource::SkinnedVertex> vertices;
	std::vector<USHORT> indices;

	FbxMesh* mesh = node->GetMesh();

	int verticesCount = mesh->GetControlPointsCount();
	for (unsigned int i = 0; i < verticesCount; ++i)
	{
		FrameResource::SkinnedVertex vertex;
		vertex.Pos.x = static_cast<float>(mesh->GetControlPointAt(i).mData[0]);
		vertex.Pos.y = static_cast<float>(mesh->GetControlPointAt(i).mData[1]);
		vertex.Pos.z = static_cast<float>(mesh->GetControlPointAt(i).mData[2]);

		vertices.push_back(vertex);
	}

	std::map<std::string, std::vector<int>> testSubset;

	int polygonCount = mesh->GetPolygonCount();
	for (unsigned int i = 0; i < polygonCount; ++i)
	{
		// normal & uv
		for (unsigned int j = 0; j < 3; ++j)
		{
			int vertexIndex = mesh->GetPolygonVertex(i, j);
			indices.emplace_back(vertexIndex);

			int k = mesh->GetElementNormalCount();
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
			}

			if (mesh->GetElementUVCount() < 1)
				continue;
			const FbxGeometryElementUV* vertexUv = mesh->GetElementUV(0);
			switch (vertexUv->GetMappingMode())
			{
			case FbxGeometryElement::eByControlPoint:
				switch (vertexUv->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					vertices[vertexIndex].TexC.x = static_cast<float>(vertexUv->GetDirectArray().GetAt(vertexIndex).mData[0]);
					vertices[vertexIndex].TexC.y = static_cast<float>(vertexUv->GetDirectArray().GetAt(vertexIndex).mData[1]);
					break;
				case FbxGeometryElement::eIndexToDirect:
					int index = vertexUv->GetIndexArray().GetAt(vertexIndex);
					vertices[vertexIndex].TexC.x = static_cast<float>(vertexUv->GetDirectArray().GetAt(index).mData[0]);
					vertices[vertexIndex].TexC.y = static_cast<float>(vertexUv->GetDirectArray().GetAt(index).mData[1]);
					break;
				}
				break;
			}
		}

		// material
		{
			int t;
			int index;
			FbxSurfaceMaterial* mat;
			std::string key;

			const FbxGeometryElementMaterial* vertexMaterial = mesh->GetElementMaterial(0);
			switch (vertexMaterial->GetMappingMode())
			{
			case FbxGeometryElement::eByPolygon:
				switch (vertexMaterial->GetReferenceMode())
				{
				case FbxGeometryElement::eIndexToDirect:
					vertexMaterial->GetDirectArray().GetCount();

					index = vertexMaterial->GetIndexArray().GetAt(i);
					mat = vertexMaterial->GetDirectArray().GetAt(index);
					if (mat)
						key = mat->GetName();
					else
						key = "unknown";
					for (int j = 0; j < 3; ++j)
						testSubset[key].push_back(i * 3 + j);
					break;
				}
				break;
			}
		}
	}

	int k = 0;
	for (auto& v : testSubset)
	{
		SubmeshGeometry s{};
		s.StartIndexLocation = v.second.front();
		s.IndexCount = v.second.back() - v.second.front();

		submeshes.push_back(s);
	}

	FbxVector4 T = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 R = node->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 S = node->GetGeometricScaling(FbxNode::eSourcePivot);
	FbxAMatrix geometryTransform = FbxAMatrix(T, R, S);

	std::map<int, std::vector<BoneWeightData>> boneWeightData;
	std::vector<XMFLOAT4X4> boneOffsets;

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
	}

	{
		const UINT vbByteSize = (UINT)vertices.size() * sizeof(FrameResource::SkinnedVertex);
		const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

		std::string name = node->GetName();
		auto mesh = std::make_unique<Mesh>();
		mesh->Name = name;

		mesh->BoneOffsets = boneOffsets;
		mesh->ParentIndexer = parentIndexer;

		ThrowIfFailed(D3DCreateBlob(vbByteSize, &mesh->VertexBufferCPU));
		CopyMemory(mesh->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

		ThrowIfFailed(D3DCreateBlob(ibByteSize, &mesh->IndexBufferCPU));
		CopyMemory(mesh->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		mesh->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(
			Graphics::Instance()->device.Get(),
			Graphics::Instance()->commandList.Get(),
			vertices.data(),
			vbByteSize,
			mesh->VertexBufferUploader);
		mesh->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(
			Graphics::Instance()->device.Get(),
			Graphics::Instance()->commandList.Get(),
			indices.data(),
			ibByteSize,
			mesh->IndexBufferUploader);

		mesh->VertexByteStride = sizeof(FrameResource::SkinnedVertex);
		mesh->VertexBufferByteSize = vbByteSize;
		mesh->IndexFormat = DXGI_FORMAT_R16_UINT;
		mesh->IndexBufferByteSize = ibByteSize;

		int materialIndex = 0;;
		for (UINT i = 0; i < submeshes.size(); ++i)
		{
			SubmeshGeometry submesh;
			std::string name = "sm_" + std::to_string(i);

			submesh.IndexCount = submeshes[i].IndexCount;
			submesh.StartIndexLocation = submeshes[i].StartIndexLocation;
			submesh.BaseVertexLocation = submeshes[i].BaseVertexLocation;
			submesh.MatIndex = materialIndex++;

			mesh->DrawArgs[name] = submesh;
		}
		Scene::scene->geometries["ApprenticeSK"] = std::move(mesh);
	}
}