#pragma once
#include "framework.h"

class Enemy : public MonoBehavior<Enemy>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<Enemy>;
	Enemy() = default;
	Enemy(Enemy&) = default;

public:
	~Enemy() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
	}

	void OnTriggerEnter(GameObject* collision)
	{
		if (!collision->GetComponent<Bullet>())
			return;
		Destroy(gameObject);
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

