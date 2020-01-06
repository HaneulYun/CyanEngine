#pragma once
#include "framework.h"

class FBX_TEST : public MonoBehavior<FBX_TEST>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
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

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};