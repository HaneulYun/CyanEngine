#pragma once
#include "framework.h"

class AfterImageGenerator : public MonoBehavior<AfterImageGenerator>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	float creationCycle{ 0.2f };
	float currentCycle{ 0.0f };

	float afterImageLifeTime{ 0.4f };

public  /*이 영역에 public 변수를 선언하세요.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<AfterImageGenerator>;
	AfterImageGenerator() = default;
	AfterImageGenerator(AfterImageGenerator&) = default;

public:
	~AfterImageGenerator() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
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

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
	void SetEntity(float _creationCycle, float _lifeTime)
	{
		creationCycle = _creationCycle;
		afterImageLifeTime = _lifeTime;
	}
};