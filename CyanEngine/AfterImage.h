#pragma once
#include "framework.h"

class AfterImage : public MonoBehavior<AfterImage>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	float lifeTime{ 1.0f };
	float currentTime{ 1.0f };
	Vector3 color{};

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
		currentTime = lifeTime;
		XMFLOAT4 _color = gameObject->GetComponent<Renderer>()->material->albedo;
		color.x = _color.x;
		color.y = _color.y;
		color.z = _color.z;
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		currentTime -= Time::deltaTime;

		float t = (currentTime / lifeTime);

		gameObject->GetComponent<Transform>()->position.z = 1 - t;
		gameObject->GetComponent<Renderer>()->material->albedo = (color * t).XMF4();

		if (currentTime < 0.0f)
			Destroy(gameObject);
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
	void SetEntity(float _lifeTime)
	{
		currentTime = lifeTime = _lifeTime;
	}
};