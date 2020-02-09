#pragma once
#include "framework.h"
#include "SkinnedData.h"


class FBX_TEST : public MonoBehavior<FBX_TEST>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	FbxManager* fbxManager{ nullptr };
	FbxScene* fbxScene{ nullptr };
	FbxImporter* fbxImporter{ nullptr };
	//FbxMesh* fbxMesh{ nullptr };
	//MeshFromFbx* mesh;

	XMFLOAT3* controlPoints = NULL;
	XMFLOAT4* colors = NULL;
	XMFLOAT3* normals = NULL;
	UINT* indices = NULL;
	UINT nVertices = 0;
	UINT nIndices = 0;

	std::vector<Bone> skeleton;

private:
	friend class GameObject;
	friend class MonoBehavior<FBX_TEST>;
	FBX_TEST() = default;
	FBX_TEST(FBX_TEST&) = default;

public:
	~FBX_TEST() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
		fbxManager = FbxManager::Create();

		FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
		fbxManager->SetIOSettings(ios);

		fbxScene = FbxScene::Create(fbxManager, "Scene");

		std::string message;
		if (fbxManager)
		{
			fbxImporter = FbxImporter::Create(fbxManager, "");

			char url[20]{ "Model/humanoid.FBX" };
			if (fbxImporter->Initialize(url, -1))
			{
				message = "Importing file ";
				message += url;
				message += "\nPlease wait!\n";
			}
			else
			{
				message = "Unable to open file ";
				message += url;
				message += "\nError reported: ";
				message += fbxImporter->GetStatus().GetErrorString();
				message += "\nEsc to exit";
			}
		}

		Debug::Log(message.c_str());

		fbxImporter->Import(fbxScene);
		fbxImporter->Destroy();

		FbxGeometryConverter geometryConverter(fbxManager);
		geometryConverter.Triangulate(fbxScene, true);

		FbxNode* rootnode = fbxScene->GetRootNode();

		ProcessNode(rootnode, FbxNodeAttribute::eSkeleton);
		ProcessNode(rootnode, FbxNodeAttribute::eMesh);

		// ���̷��� ���� �ҷ�����,
		// �޽� �ҷ�����,
		// ���̷��濡 �ִϸ��̼� ���� �ֱ�

	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{

	}

	void ProcessNode(FbxNode* node, FbxNodeAttribute::EType type) {
		if (node)
		{
			if (node->GetNodeAttribute() != NULL)
			{
				FbxNodeAttribute::EType AttributeType = node->GetNodeAttribute()->GetAttributeType();

				switch (AttributeType)
				{
				case FbxNodeAttribute::eUnknown:
					break;
				case FbxNodeAttribute::eNull:
					break;
				case FbxNodeAttribute::eMarker:
					break;
				case FbxNodeAttribute::eSkeleton:
					if (skeleton.empty())
						ProcessSkeletonHierarchy(node);
					break;
				case FbxNodeAttribute::eMesh:
					ProcessMesh(node);
					return;
				case FbxNodeAttribute::eNurbs:
					break;
				case FbxNodeAttribute::ePatch:
					break;
				case FbxNodeAttribute::eCamera:
					break;
				case FbxNodeAttribute::eCameraStereo:
					break;
				case FbxNodeAttribute::eCameraSwitcher:
					break;
				case FbxNodeAttribute::eLight:
					break;
				case FbxNodeAttribute::eOpticalReference:
					break;
				case FbxNodeAttribute::eOpticalMarker:
					break;
				case FbxNodeAttribute::eNurbsCurve:
					break;
				case FbxNodeAttribute::eTrimNurbsSurface:
					break;
				case FbxNodeAttribute::eBoundary:
					break;
				case FbxNodeAttribute::eNurbsSurface:
					break;
				case FbxNodeAttribute::eShape:
					break;
				case FbxNodeAttribute::eLODGroup:
					break;
				case FbxNodeAttribute::eSubDiv:
					break;
				case FbxNodeAttribute::eCachedEffect:
					break;
				case FbxNodeAttribute::eLine:
					break;
				default:
					break;
				}
			}

			const int childCount = node->GetChildCount();

			for (int i = 0; i < childCount; ++i)
 				ProcessNode(node->GetChild(i), type);
		}
	}

	void ProcessMesh(FbxNode* node)
	{
		FbxMesh* fbxMesh = node->GetMesh();
		MeshFromFbx* mesh = new MeshFromFbx(fbxMesh, skeleton);

		gameObject->AddComponent<MeshFilter>()->mesh = mesh;
		gameObject->AddComponent<Renderer>()->material = new DefaultMaterial();
	}

	void ProcessSkeletonHierarchy(FbxNode* node)
	{
		for (int childIndex = 0; childIndex < node->GetChildCount(); ++childIndex)
		{
			FbxNode* childNode = node->GetChild(childIndex);
			ProcessSkeletonHierarchyRecursively(childNode, 0, -1);
		}
	}

	// inDepth is not needed here, I used it for debug but forgot to remove it
	void ProcessSkeletonHierarchyRecursively(FbxNode* node, int myIndex, int parentIndex)
	{
		if (node->GetNodeAttribute() && node->GetNodeAttribute()->GetAttributeType() && node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			Bone bone;
			bone.parentIndex = parentIndex;
			bone.name = node->GetName();
			skeleton.push_back(bone);
		}
		for (int i = 0; i < node->GetChildCount(); i++)
		{
			ProcessSkeletonHierarchyRecursively(node->GetChild(i), skeleton.size(), myIndex);
		}

	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};