#pragma once
#include "framework.h"

class FBX_TEST : public MonoBehavior<FBX_TEST>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	FbxManager* fbxManager{ nullptr };
	FbxScene* fbxScene{ nullptr };
	FbxImporter* fbxImporter{ nullptr };
	FbxMesh* fbxMesh{ nullptr };

	XMFLOAT3* controlPoints = NULL;
	XMFLOAT4* colors = NULL;
	XMFLOAT3* normals = NULL;
	UINT* indices = NULL;

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
		if (!fbxManager)
		{
			Debug::Log("Error: Unable to create FBX Manager!\n");
		}
		else
		{
			char str[100];
			sprintf(str, "Autodesk FBX SDK version %s\n", fbxManager->GetVersion());
			Debug::Log(str);
		}

		FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
		fbxManager->SetIOSettings(ios);

		fbxScene = FbxScene::Create(fbxManager, "Scene");
		if (!fbxScene)
		{
			Debug::Log("Error: Unable to create FBX scene!\n");
		}

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

				// Set scene status flag to ready to load.
				//mStatus = MUST_BE_LOADED;
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
		else
		{
			message = "Unable to create the FBX SDK manager";
			message += "\nEsc to exit";
		}

		Debug::Log(message.c_str());

		fbxImporter->Import(fbxScene);
		
		fbxImporter->Destroy();


		FbxGeometryConverter geometryConverter(fbxManager);
		geometryConverter.Triangulate(fbxScene, true);

		LoadMesh();
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{

	}

	void FindMesh(FbxNode* node)
	{
		FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();

		if (nodeAttribute) {
			if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				fbxMesh = node->GetMesh();
				Debug::Log("Finding Mesh Success\n");
				return;
			}
		}
		const int childCount = node->GetChildCount();

		for (unsigned int i = 0; i < childCount; ++i)
			FindMesh(node->GetChild(i));
	};

	void ProcessControlPoints(FbxMesh* mesh)
	{
		int count = mesh->GetControlPointsCount();
		controlPoints = new XMFLOAT3[count];

		for (int i = 0; i < count; ++i)
		{
			XMFLOAT3 pos;
			pos.x = static_cast<float>(mesh->GetControlPointAt(i).mData[0]);
			pos.y = static_cast<float>(mesh->GetControlPointAt(i).mData[1]);
			pos.z = static_cast<float>(mesh->GetControlPointAt(i).mData[2]);

			controlPoints[i] = pos;
		}
	}

	void LoadMesh()
	{
		FbxNode* rootNode = fbxScene->GetRootNode();
		FindMesh(rootNode);

		// 제어점 위치 찾기
		ProcessControlPoints(fbxMesh);

		// 제어점 인덱스 찾기
		int triangleCount = fbxMesh->GetPolygonCount();
		int vertexCount = 0;

		indices = new UINT[triangleCount * 3];


		for (int i = 0; i < triangleCount; ++i) 
		{
			for (int j = 0; j < 3; ++j)
			{
				int controlPointIndex = fbxMesh->GetPolygonVertex(i, j);
				indices[vertexCount++] = controlPointIndex;
			}
		}
	
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};