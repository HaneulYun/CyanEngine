#pragma once

class ModelManager : public Singleton<ModelManager>
{
private:
	std::map<std::string, GameObject*> database;

	FbxManager* fbxManager{ nullptr };
	FbxScene* fbxScene{ nullptr };
	FbxImporter* fbxImporter{ nullptr };

public:
	GameObject* LoadGeometryFromFBX(const char* pstrFileName);
};