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
		scene->SetCurrentAnimationStack(animStack);

		//FbxAnimLayer* animLayer = animStack->GetMember<FbxAnimLayer>();

		FbxTakeInfo* takeInfo = scene->GetTakeInfo(*(animStackNameArray[stackIndex]));
		FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
		FbxTime end = takeInfo->mLocalTimeSpan.GetStop();

		for (unsigned int j = 0; j < clip->BoneAnimations.size(); ++j)
		{
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
			clip->BoneAnimations[j] = boneAnim;
		}

		for (int j = 0; j < clip->BoneAnimations.size(); ++j)
		{
			BoneAnimation boneAnim;
			if (clip->BoneAnimations[j].Keyframes.size() == 0)
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
				clip->BoneAnimations[j] = boneAnim;
			}
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
	std::vector<M3DLoader::SkinnedVertex> vertices;
	std::vector<USHORT> indices;

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

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(FrameResource::SkinnedVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	std::string mSkinnedModelFilename = "Models\\soldier.m3d";

	auto geo = std::make_unique<Mesh>();
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

	skinnedSubsets.resize(1);

	for (UINT i = 0; i < 1; ++i)
	{
		skinnedSubsets[i].Id = 0;
		skinnedSubsets[i].VertexStart = 0;
		skinnedSubsets[i].VertexCount = vertices.size();
		skinnedSubsets[i].FaceStart = 0;
		skinnedSubsets[i].FaceCount = indices.size();
	}
	for (UINT i = 0; i < (UINT)skinnedSubsets.size(); ++i)
	{
		SubmeshGeometry submesh;
		std::string name = "sm_" + std::to_string(i);

		submesh.IndexCount = (UINT)skinnedSubsets[i].FaceCount * 3;
		submesh.StartIndexLocation = skinnedSubsets[i].FaceStart * 3;
		submesh.BaseVertexLocation = 0;

		geo->DrawArgs[name] = submesh;
	}

	Scene::scene->geometries[geo->Name] = std::move(geo);

}