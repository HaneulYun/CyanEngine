#include "pch.h"
#include "LoadFbx.h"

void LoadFbx::LoadFbxFileName(const char* fileName) 
{
	// Fbx 변수 선언
	FbxManager* fbxManager{ nullptr };
	FbxScene* fbxScene{ nullptr };
	FbxImporter* fbxImporter{ nullptr };

	fbxManager = FbxManager::Create();

	FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ios);

	fbxScene = FbxScene::Create(fbxManager, "Scene");

	if (fbxManager)
	{
		fbxImporter = FbxImporter::Create(fbxManager, "");
		fbxImporter->Initialize(fileName, -1);
	}

	fbxImporter->Import(fbxScene);
	//fbxImporter->Destroy();

	FbxGeometryConverter geometryConverter(fbxManager);
	geometryConverter.Triangulate(fbxScene, true);

	FbxNode* rootnode = fbxScene->GetRootNode();

	ProcessNode(rootnode, FbxNodeAttribute::eSkeleton);
	ProcessNode(rootnode, FbxNodeAttribute::eMesh);

	/*

	// fbx 파일 내에서 설정된 애니메이션 FrameRate 로드
	float frameRate = (float)FbxTime::GetFrameRate(fbxScene->GetGlobalSettings().GetTimeMode());

	// fbx 파일에 저장된 모든 Animation의 이름 로드
	FbxArray<FbxString*> animationArray;
	FbxDocument* document = dynamic_cast<FbxDocument*>(fbxScene);
	if (document != NULL)
		document->FillAnimStackNameArray(animationArray);

	// importer로 부터 fbx에 저장된 모든 Animation 정보를 로드
	for (int i = 0; i < fbxImporter->GetAnimStackCount(); ++i)
	{
		FbxTakeInfo* animationInfo = fbxImporter->GetTakeInfo(i);
		std::string animationName = animationInfo->mName.Buffer();

		// fbxTimeSpan : 구간 시간을 저장하는 Class
		FbxTimeSpan span = animationInfo->mLocalTimeSpan;

		// 시작, 끝 시간을 로드
		float startTime = span.GetStart().GetSecondDouble();
		float endTime = span.GetStop().GetSecondDouble();

		if (startTime < endTime)
		{
			// KeyFrame의 수 = Animation 실행 시간(초) * 초당 Frame 수 + 1(0 프레임)
			int numOfFrames = (int)((endTime - startTime) * (double)frameRate);

			// 여기까지 애니메이션 이름 / 키 프레임의 갯수 / 프레임 레이트
			// 이 정보로 애니메이션마다 각 Bone의 변환정보를 불러와야 한다.
			
			AnimationClip clip;
			clip.BoneAnimations.resize(skeleton.size());

			ProcessAnimation(rootnode, animationName, endTime, frameRate, clip);

			animations[animationName] = clip;

		}
	}
	SkinnedData* skinnedInfo = new SkinnedData;
	skinnedInfo->Set(boneHierarchy, boneOffsets, animations);
	AnimationController* animationController = gameObject->AddComponent<AnimationController>();
	animationController->SkinnedInfo = skinnedInfo;
	animationController->FinalTransforms.resize(skinnedInfo->BoneCount());
	animationController->ClipName = "Unreal Take";
	animationController->TimePos = 0.0f;
	*/
}


void LoadFbx::LoadAniFileName(const char* fileName)
{
	// Fbx 변수 선언
	FbxManager* fbxManager{ nullptr };
	FbxScene* fbxScene{ nullptr };
	FbxImporter* fbxImporter{ nullptr };

	fbxManager = FbxManager::Create();

	FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ios);

	fbxScene = FbxScene::Create(fbxManager, "Scene");

	if (fbxManager)
	{
		fbxImporter = FbxImporter::Create(fbxManager, "");
		fbxImporter->Initialize(fileName, -1);
	}

	fbxImporter->Import(fbxScene);
	//fbxImporter->Destroy();

	FbxGeometryConverter geometryConverter(fbxManager);
	geometryConverter.Triangulate(fbxScene, true);

	FbxNode* rootnode = fbxScene->GetRootNode();

	// fbx 파일 내에서 설정된 애니메이션 FrameRate 로드
	float frameRate = (float)FbxTime::GetFrameRate(fbxScene->GetGlobalSettings().GetTimeMode());

	// fbx 파일에 저장된 모든 Animation의 이름 로드
	FbxArray<FbxString*> animationArray;
	FbxDocument* document = dynamic_cast<FbxDocument*>(fbxScene);
	if (document != NULL)
		document->FillAnimStackNameArray(animationArray);

	// importer로 부터 fbx에 저장된 모든 Animation 정보를 로드
	for (int i = 0; i < fbxImporter->GetAnimStackCount(); ++i)
	{
		FbxTakeInfo* animationInfo = fbxImporter->GetTakeInfo(i);
		std::string animationName = animationInfo->mName.Buffer();

		// fbxTimeSpan : 구간 시간을 저장하는 Class
		FbxTimeSpan span = animationInfo->mLocalTimeSpan;

		// 시작, 끝 시간을 로드
		float startTime = span.GetStart().GetSecondDouble();
		float endTime = span.GetStop().GetSecondDouble();

		if (startTime < endTime)
		{
			// KeyFrame의 수 = Animation 실행 시간(초) * 초당 Frame 수 + 1(0 프레임)
			int numOfFrames = (int)((endTime - startTime) * (double)frameRate);

			// 여기까지 애니메이션 이름 / 키 프레임의 갯수 / 프레임 레이트
			// 이 정보로 애니메이션마다 각 Bone의 변환정보를 불러와야 한다.

			AnimationClip clip;
			clip.BoneAnimations.resize(skeleton.size());

			ProcessAnimation(rootnode, animationName, endTime, frameRate, clip);

			animations[animationName] = clip;

		}
	}
	SkinnedData* skinnedInfo = new SkinnedData;
	skinnedInfo->Set(boneHierarchy, boneOffsets, animations);
	AnimationController* animationController = gameObject->AddComponent<AnimationController>();
	animationController->SkinnedInfo = skinnedInfo;
	animationController->FinalTransforms.resize(skinnedInfo->BoneCount());
	animationController->ClipName = "Unreal Take";
	animationController->TimePos = 0.0f;
}


void LoadFbx::ProcessNode(FbxNode* node, FbxNodeAttribute::EType type)
{
	if (node)
	{
		if (node->GetNodeAttribute() != NULL)
		{
			FbxNodeAttribute::EType AttributeType = node->GetNodeAttribute()->GetAttributeType();

			if (type == FbxNodeAttribute::eSkeleton && AttributeType == FbxNodeAttribute::eSkeleton)
			{
				ProcessSkeleton(node);
				return;
			}
				
			if (type == FbxNodeAttribute::eMesh && AttributeType == FbxNodeAttribute::eMesh)
			{
				ProcessMesh(node);
				return;
			}
			
		}
		//const int childCount = node->GetChildCount();

		for (int i = 0; i < node->GetChildCount(); ++i)
			ProcessNode(node->GetChild(i), type);

	}
}

void LoadFbx::ProcessSkeleton(FbxNode* node)
{
	ProcessSkeletonHierarchyRecursively(node, 0, -1);

	//for (int childIndex = 0; childIndex < node->GetChildCount(); ++childIndex)
	//{
	//	FbxNode* childNode = node->GetChild(childIndex);
	//	ProcessSkeletonHierarchyRecursively(childNode, 0, -1);
	//}
}

void LoadFbx::ProcessSkeletonHierarchyRecursively(FbxNode* node, int myIndex, int parentIndex)
{
	if (node->GetNodeAttribute() && node->GetNodeAttribute()->GetAttributeType() && node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		Bone bone;
		bone.parentIndex = parentIndex;
		bone.name = node->GetName();
		skeleton.push_back(bone);
		boneHierarchy.push_back(parentIndex);

	}
	for (int i = 0; i < node->GetChildCount(); i++)
	{
		ProcessSkeletonHierarchyRecursively(node->GetChild(i), skeleton.size(), myIndex);
	}

}

void LoadFbx::ProcessMesh(FbxNode* node)
{
	boneOffsets.resize(skeleton.size());

	// Mesh Load
	FbxMesh* fbxMesh = node->GetMesh();
	MeshFromFbx* mesh = new MeshFromFbx(fbxMesh, skeleton);

	gameObject->AddComponent<MeshFilter>()->mesh = mesh;
	//gameObject->AddComponent<Renderer>()->material = new DefaultMaterial();
	gameObject->AddComponent<Renderer>()->material = new Material(L"Texture/Flower01.dds", ShaderMode::Skinned);

	// Bone Matrix Load
	// Offset Matrix와 Animation을 위한 LoaclMatrix를 구한다. (Hierarchy)
	int numOfDeformers = fbxMesh->GetDeformerCount();
	FbxSkin* fbxSkin = reinterpret_cast<FbxSkin*>(fbxMesh->GetDeformer(0 /*Deformer Index*/, FbxDeformer::eSkin));


	const FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = node->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = node->GetGeometricScaling(FbxNode::eSourcePivot);

	FbxAMatrix geometryTransform = FbxAMatrix(lT, lR, lS);

	// Cluster 
	if (fbxSkin != NULL)
	{
		for (int i = 0; i < fbxSkin->GetClusterCount(); ++i)
		{
			FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

			// 이 Cluster에 상응하는 Skeleton 노드를 얻어온다.
			std::string BoneName = fbxCluster->GetLink()->GetName();

			// 해당 Skeleton의 index를 가져온다.
			auto bone = std::find_if(skeleton.begin(), skeleton.end(), [BoneName](const Bone& a) { return a.name.compare(BoneName) == 0; });
			int boneIndex = std::distance(skeleton.begin(), bone);

			// Bone Offset
			FbxAMatrix matTransform;
			FbxAMatrix matLinkTransform;
			FbxAMatrix matBoneOffsetMatrix;

			// Frame-Space의 Local 매트릭스
			fbxCluster->GetTransformMatrix(matTransform);
			// Frame-Space에서 World-Space으로 변환하는 매트릭스 show me how
			fbxCluster->GetTransformLinkMatrix(matLinkTransform);
			// World-Space에서 Frame-Space로 변환하는 매트릭스 (Offset)
			matBoneOffsetMatrix = matLinkTransform.Inverse() * matTransform * geometryTransform;
			FbxAMatrix m = matBoneOffsetMatrix.Inverse();
			boneOffsets[boneIndex] = ToXMfloat4x4(m);
		}
		int a = 0;
	}
	//@@ Cluster에 없는 Bone Offset도 구해주어야 할듯??? @@
}

void LoadFbx::ProcessAnimation(FbxNode* node, std::string animationName, float stop, float frameRate, AnimationClip& clip)
{
	if (node)
	{
		if (node->GetNodeAttribute() != NULL)
		{
			FbxNodeAttribute::EType AttributeType = node->GetNodeAttribute()->GetAttributeType();

			if (AttributeType == FbxNodeAttribute::eSkeleton)
			{
				std::string boneName = node->GetName();
				auto bone = std::find_if(skeleton.begin(), skeleton.end(), [boneName](const Bone& a) { return a.name.compare(boneName) == 0; });
				int boneIndex = std::distance(skeleton.begin(), bone);

				double time = 0;

				while (time <= (double)stop)
				{
					FbxTime animationTime;
					animationTime.SetSecondDouble(time);

					// Local Transform
					Keyframe keyframe;
					keyframe.TimePos = time;
					keyframe.Translation = ToXMfloat3(node->EvaluateLocalTranslation(time));
					keyframe.Scale = ToXMfloat3(node->EvaluateLocalScaling(time));
					keyframe.RotationQuat = ToXMfloat4(node->EvaluateLocalRotation(time));

					clip.BoneAnimations[boneIndex].Keyframes.push_back(keyframe);

					time += 1.0f / frameRate;
				}
			}
		}

		for (int i = 0; i < node->GetChildCount(); ++i)
			ProcessAnimation(node->GetChild(i), animationName, stop, frameRate, clip);
	}

}

XMFLOAT4X4 LoadFbx::ToXMfloat4x4(FbxAMatrix& fbxMatrix)
{
	FbxVector4 r1 = fbxMatrix.GetRow(0);
	FbxVector4 r2 = fbxMatrix.GetRow(1);
	FbxVector4 r3 = fbxMatrix.GetRow(2);
	FbxVector4 r4 = fbxMatrix.GetRow(3);

	return XMFLOAT4X4
	{
		(float)r1.mData[0], (float)r1.mData[1], (float)r1.mData[2], (float)r1.mData[3],
		(float)r2.mData[0], (float)r2.mData[1], (float)r2.mData[2], (float)r2.mData[3],
		(float)r3.mData[0], (float)r3.mData[1], (float)r3.mData[2], (float)r3.mData[3],
		(float)r4.mData[0], (float)r4.mData[1], (float)r4.mData[2], (float)r4.mData[3]
	};
}

XMFLOAT3 LoadFbx::ToXMfloat3(FbxVector4& fbxVector4)
{
	return XMFLOAT3
	{
		(float)fbxVector4.mData[0],
		(float)fbxVector4.mData[1],
		(float)fbxVector4.mData[2]
	};
}

XMFLOAT4 LoadFbx::ToXMfloat4(FbxVector4& fbxVector4)
{
	return XMFLOAT4
	{
		(float)fbxVector4.mData[0],
		(float)fbxVector4.mData[1],
		(float)fbxVector4.mData[2],
		(float)fbxVector4.mData[3]
	};
}