#pragma once
#include "framework.h"

class FBX_TEST : public MonoBehavior<FBX_TEST>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	FbxManager* fbxManager{ nullptr };
	FbxScene* fbxScene{ nullptr };
	FbxImporter* fbxImporter{ nullptr };

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
				message += "\nPlease wait!";

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
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};