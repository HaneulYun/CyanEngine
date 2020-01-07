#include "pch.h"
#include "GameScene.h"

Scene* Scene::scene{ nullptr };

void GameScene::BuildObjects()
{
	scene = this;

	GameObject* mainCamera = CreateEmpty();
	{
		Camera* camera = mainCamera->AddComponent<Camera>();
		camera->SetViewport(0, 0, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight, 0.0f, 1.0f);
		camera->SetScissorRect(0, 0, CyanWindow::m_nWndClientWidth, CyanWindow::m_nWndClientHeight);
		camera->GenerateProjectionMatrix(0.3f, 10000.0f, float(CyanWindow::m_nWndClientWidth) / float(CyanWindow::m_nWndClientHeight), 90.0f);
		camera->GenerateViewMatrix(XMFLOAT3(0.0f, 10.0f, -15.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		camera->main = camera;
	}

	GameObject* object = CreateEmpty();
	{
		object->AddComponent<FBX_TEST>();
	}

	GameObject* player = CreateEmpty();
	{
		FbxManager* fbxManager{ nullptr };
		FbxScene* fbxScene{ nullptr };
		FbxImporter* fbxImporter{ nullptr };

		fbxManager = FbxManager::Create();
		FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
		fbxManager->SetIOSettings(ios);

		fbxScene = FbxScene::Create(fbxManager, "Scene");

		fbxImporter = FbxImporter::Create(fbxManager, "");
		fbxImporter->Initialize("Model/humanoid.fbx", -1);
		fbxImporter->Import(fbxScene);

		FbxNode* rootNode = fbxScene->GetRootNode();

		if (rootNode)
		{
			for (int i = 0; i < rootNode->GetChildCount(); ++i)
			{
				FbxNode* childNode = rootNode->GetChild(i);

				if (!childNode->GetNodeAttribute())
					continue;

				FbxNodeAttribute::EType AttributeType = childNode->GetNodeAttribute()->GetAttributeType();

				if (AttributeType != FbxNodeAttribute::eMesh)
					continue;

				FbxMesh* fbxMesh = (FbxMesh*)childNode->GetNodeAttribute();
				player->AddComponent<MeshFilter>()->mesh = new MeshFromFbx(fbxMesh);
			}
		}
		player->GetComponent<Transform>()->position = { 0, 0, 0 };
		player->GetComponent<Transform>()->Scale({ 0.05f, 0.05f, 0.05f });
		player->AddComponent<Renderer>()->material = new Material();

		//GameObject* child = ModelManager::Instance()->LoadGeometryFromFBX("Model/humanoid.fbx");
		//child->GetComponent<Transform>()->Scale({ 0.2f, 0.2f, 0.2f });
		//player->AddChild(child);

		player->AddComponent<Controller>()->gameObject = player;
	}
}