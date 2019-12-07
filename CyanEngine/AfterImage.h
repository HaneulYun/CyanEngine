#pragma once
#include "framework.h"

class AfterImage : public MonoBehavior<AfterImage>
{
private /*�� ������ private ������ �����ϼ���.*/:
	float lifeTime{ 0.0f };

public  /*�� ������ public ������ �����ϼ���.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<AfterImage>;
	AfterImage() = default;
	AfterImage(AfterImage&) = default;

public:
	~AfterImage() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		lifeTime -= Time::deltaTime;
		if (lifeTime < 0.0f)
			Destroy(gameObject);
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};