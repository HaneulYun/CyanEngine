#pragma once
#include "framework.h"

class FBX_TEST : public MonoBehavior<FBX_TEST>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	FbxManager* fbxManager{ nullptr };
	FbxScene* fbxScene{ nullptr };

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
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};