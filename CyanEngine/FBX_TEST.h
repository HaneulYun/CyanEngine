#pragma once
#include "framework.h"

class FBX_TEST : public MonoBehavior<FBX_TEST>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	FbxManager* fbxManager{ nullptr };
	FbxScene* fbxScene{ nullptr };
	FbxImporter* fbxImporter{ nullptr };
	//FbxMesh* fbxMesh{ nullptr };

	XMFLOAT3* controlPoints = NULL;
	XMFLOAT4* colors = NULL;
	XMFLOAT3* normals = NULL;
	UINT* indices = NULL;
	UINT nVertices = 0;
	UINT nIndices = 0;


private:
	friend class GameObject;
	friend class MonoBehavior<FBX_TEST>;
	FBX_TEST() = default;
	FBX_TEST(FBX_TEST&) = default;

public:
	~FBX_TEST() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
		fbxManager = FbxManager::Create();

		FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
		fbxManager->SetIOSettings(ios);

		fbxScene = FbxScene::Create(fbxManager, "Scene");

		std::string message;
		if (fbxManager)
		{
			fbxImporter = FbxImporter::Create(fbxManager, "");

			char url[20]{ "humanoid.fbx" };
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

		ProcessNode(rootnode);
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{

	}

	void ProcessNode(FbxNode* rootnode) {

		if (rootnode)
		{
			for (int i = 0; i < rootnode->GetChildCount(); i++)
			{
				FbxNode* childNode = rootnode->GetChild(i);

				if (childNode->GetNodeAttribute() == NULL)
					continue;

				FbxNodeAttribute::EType AttributeType = childNode->GetNodeAttribute()->GetAttributeType();

				switch (AttributeType)
				{
				case FbxNodeAttribute::eUnknown:
					break;
				case FbxNodeAttribute::eNull:
					break;
				case FbxNodeAttribute::eMarker:
					break;
				case FbxNodeAttribute::eSkeleton:
					break;
				case FbxNodeAttribute::eMesh:
					ReadMesh(childNode);
					break;
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
		}
	}

	void ReadMesh(FbxNode* node)
	{
		FbxMesh* fbxMesh = node->GetMesh();
		MeshFromFbx* mesh = new MeshFromFbx(fbxMesh);

		gameObject->AddComponent<MeshFilter>()->mesh = mesh;
		gameObject->AddComponent<Renderer>()->material = new DefaultMaterial();

		/*

		FbxMesh* fbxMesh = node->GetMesh();

		// Vertex Data
		int vertexCnt = fbxMesh->GetControlPointsCount();
		nVertices = vertexCnt;

		controlPoints = new XMFLOAT3[vertexCnt];

		for (int i = 0; i < vertexCnt; ++i)
		{
			XMFLOAT3 pos;
			pos.x = static_cast<float>(fbxMesh->GetControlPointAt(i).mData[0]);
			pos.y = static_cast<float>(fbxMesh->GetControlPointAt(i).mData[1]);
			pos.z = static_cast<float>(fbxMesh->GetControlPointAt(i).mData[2]);

			controlPoints[i] = pos;
		}

		// Index Data
		int triangleCnt = fbxMesh->GetPolygonCount();
		int vertexCount = 0;

		UINT nIndices = triangleCnt * 3;
		indices = new UINT[nIndices];

		for (int i = 0; i < triangleCnt; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				int controlPointIndex = fbxMesh->GetPolygonVertex(i, j);
				indices[vertexCount++] = controlPointIndex;
			}
		}

		*/

	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};