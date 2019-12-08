#pragma once
#include "framework.h"

class AfterImageGenerator : public MonoBehavior<AfterImageGenerator>
{
private /*�� ������ private ������ �����ϼ���.*/:
	float creationCycle{ 0.2f };
	float currentCycle{ 0.0f };

	float afterImageLifeTime{ 0.4f };

public  /*�� ������ public ������ �����ϼ���.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<AfterImageGenerator>;
	AfterImageGenerator() = default;
	AfterImageGenerator(AfterImageGenerator&) = default;

public:
	~AfterImageGenerator() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		currentCycle -= Time::deltaTime;
		if (currentCycle < 0)
		{
			GameObject* vfx = new GameObject();
			*vfx->GetComponent<Transform>() = *gameObject->GetComponent<Transform>();
			vfx->GetComponent<Transform>()->gameObject = vfx;
			vfx->AddComponent<MeshFilter>(gameObject->GetComponent<MeshFilter>());
			vfx->AddComponent<Renderer>(gameObject->GetComponent<Renderer>());
			vfx->AddComponent<AfterImage>()->SetEntity(afterImageLifeTime);

			currentCycle += creationCycle;
		}
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
	void SetEntity(float _creationCycle, float _lifeTime)
	{
		creationCycle = _creationCycle;
		afterImageLifeTime = _lifeTime;
	}
};