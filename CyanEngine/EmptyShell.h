#pragma once
#include "framework.h"

class EmptyShell : public MonoBehavior<EmptyShell>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	float lifeTime{ 1.0f };
	float currentTime{ 1.0f };
	float maxSpeed{ 100.0f };
	Vector3 direction{};

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
		// EmptyShell 은 SetEntity 를 통해 초기값을 설정하여하 합니다.
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		currentTime -= Time::deltaTime;
		float t = (currentTime / lifeTime);
		if (currentTime < 0.0f)
			Destroy(gameObject);

		float speed = maxSpeed * t * t;
		gameObject->transform->position += direction * speed * Time::deltaTime;
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
	void SetEntity(const Vector3& _direction, float _lifeTime)
	{
		currentTime = lifeTime = _lifeTime;
		direction = _direction;
	}
};