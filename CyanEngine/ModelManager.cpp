#include "pch.h"
#include "ModelManager.h"

GameObject* ModelManager::LoadGeometryFromFBX(const char* pstrFileName)
{
	auto iter = database.find(pstrFileName);
	if (iter != database.end())
	{
		GameObject* gameObject = new GameObject(iter->second);
		gameObject->GetComponent<Renderer>()->material->DiffuseAlbedo = RANDOM_COLOR;
		for (auto iter : gameObject->children)
			iter->GetComponent<Renderer>()->material = gameObject->GetComponent<Renderer>()->material;
		return gameObject;
	}

	GameObject* pGameObject = nullptr;
	

	fbxManager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ios);

	fbxScene = FbxScene::Create(fbxManager, "Scene");

	fbxImporter = FbxImporter::Create(fbxManager, "");
	fbxImporter->Initialize(pstrFileName, -1);
	fbxImporter->Import(fbxScene);


	database[pstrFileName] = pGameObject;

	return new GameObject(pGameObject);
}
