#pragma once
#include "framework.h"
#include "Bullet.h"

class StarGuardian : public Component
{
private:
	// 이 영역에 private 변수를 선언하세요.

public:
	// 이 영역에 public 변수를 선언하세요.
	GameObject* bullet{ nullptr };

private:
	friend class GameObject;
	StarGuardian() {}
	StarGuardian(StarGuardian&) = default;

public:
	~StarGuardian() {}
	virtual Component* Duplicate() { return new StarGuardian; };
	virtual Component* Duplicate(Component* component) { return new StarGuardian(*(StarGuardian*)component); }

	void Starguadiant()
	{
		// 초기화 코드를 작성하세요.
	}

	void Update()
	{
		// 업데이트 코드를 작성하세요.
		
	}

	void Shoot(int type)
	{
		//if (Input::GetMouseButtonDown(0)) {
			GameObject* object = Instantiate(bullet);
			Vector3 mousepostion = Camera::main->ScreenToWorldPoint(Input::mousePosition);

			object->transform->position = gameObject->transform->position;
			Vector3 direction = mousepostion - object->transform->position;
			direction.z = 0;
			object->GetComponent<Bullet>()->direction = direction;
		//}
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

