#pragma once
#include "framework.h"

class FBX_TEST : public MonoBehavior<FBX_TEST>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	

private:
	friend class GameObject;
	friend class MonoBehavior<FBX_TEST>;
	FBX_TEST() = default;
	FBX_TEST(FBX_TEST&) = default;

public:
	~FBX_TEST() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};