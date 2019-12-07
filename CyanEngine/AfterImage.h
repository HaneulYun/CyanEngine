#pragma once
#include "framework.h"

class AfterImage : public MonoBehavior<AfterImage>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	float lifeTime{ 0.0f };

public  /*이 영역에 public 변수를 선언하세요.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<AfterImage>;
	AfterImage() = default;
	AfterImage(AfterImage&) = default;

public:
	~AfterImage() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		lifeTime -= Time::deltaTime;
		if (lifeTime < 0.0f)
			Destroy(gameObject);
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};