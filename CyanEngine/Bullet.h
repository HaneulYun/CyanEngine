#pragma once
#include "framework.h"

class Enemy;

class Bullet : public MonoBehavior<Bullet>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	float timeCycle{ 0.5f };
	float speed{ 200.f };
	Vector3 direction;

private:
	friend class GameObject;
	friend class MonoBehavior<Bullet>;
	Bullet() = default;
	Bullet(Bullet&) = default;

public:
	~Bullet() {}

	void Start()
	{
		// 초기화 코드를 작성하세요.
		direction = direction.Normalize();
		gameObject->transform->Rotate({ 0, 0, 1 }, direction.Degree() - 90);
	}

	void Update()
	{
		Vector3 movevector = direction * speed * Time::deltaTime;
		gameObject->transform->position = gameObject->transform->position + movevector;
	}

	void OnTriggerEnter(GameObject* collision)
	{
		//if (!collision->GetComponent<Enemy>())
		//	return;
		//Destroy(gameObject);
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
	void SetEntity(const float _timeCycle, const float _speed, const Vector3& _direction = {})
	{
		timeCycle = _timeCycle;
		speed = _speed;
		direction = _direction;
	}
};