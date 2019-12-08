#pragma once
#include "framework.h"

class AfterImage : public MonoBehavior<AfterImage>
{
private /*�� ������ private ������ �����ϼ���.*/:
	float lifeTime{ 1.0f };
	float currentTime{ 1.0f };

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
		currentTime = lifeTime;
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		currentTime -= Time::deltaTime;

		float t = (currentTime / lifeTime);

		gameObject->GetComponent<Transform>()->position.z = 1 - t;
		gameObject->GetComponent<Renderer>()->material->albedo = {t, t, t, 1};

		if (currentTime < 0.0f)
			Destroy(gameObject);
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
	void SetEntity(float _lifeTime)
	{
		currentTime = lifeTime = _lifeTime;
	}
};