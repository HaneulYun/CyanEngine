#pragma once
#include "framework.h"

class EmptyShell : public MonoBehavior<EmptyShell>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	float lifeTime{ 1.0f };
	float currentTime{ 1.0f };
	float maxSpeed{ 100.0f };
	float rotateSpeed{ 360.0f };
	Vector3 direction{};

	XMFLOAT4X4 oldTransform{};

public  /*이 영역에 public 변수를 선언하세요.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<EmptyShell>;
	EmptyShell() = default;
	EmptyShell(EmptyShell&) = default;

public:
	~EmptyShell() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
		oldTransform = gameObject->transform->localToWorldMatrix;
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		currentTime -= Time::deltaTime;
		float t = (currentTime / lifeTime);
		if (currentTime < 0.0f)
			Destroy(gameObject);

		float speed = maxSpeed * t * t;
		float scale = sinf(t);
		gameObject->transform->localToWorldMatrix = oldTransform;
		gameObject->transform->position += direction * speed * Time::deltaTime;
		gameObject->transform->Rotate({ 0,0,1 }, rotateSpeed * Time::deltaTime);
		oldTransform = gameObject->transform->localToWorldMatrix;

		gameObject->transform->Scale({ scale, scale, 1 });
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
	void SetEntity(const Vector3& _direction, float _lifeTime)
	{
		currentTime = lifeTime = _lifeTime;
		direction = _direction;

		direction.DegreeToVector3((((int)direction.Degree() + 180 + Random::Range(-45, 45)) % 360) - 180);
		maxSpeed += Random::Range(-30.0f, 30.0f);
		rotateSpeed += Random::Range(0.0f, 360.0f);
	}
};