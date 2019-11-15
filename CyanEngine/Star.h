#pragma once
#include "framework.h"
#include "Bullet.h"

class Star : public Component
{
private:
	// 이 영역에 private 변수를 선언하세요.
	int health;

public:
	// 이 영역에 public 변수를 선언하세요.
	GameObject* bullet{ nullptr };

private:
	friend class GameObject;
	Star() {}

public:
	~Star() {}
	virtual Component* Duplicate() { return new Star; };

	void Start()
	{
		// 초기화 코드를 작성하세요.
	}

	void Update()
	{
		// 업데이트 코드를 작성하세요.
		if (Input::GetMouseButtonDown(0)) {
			GameObject* object = Instantiate(bullet);
			object->GetComponent<Bullet>()->direction = Vector3(Input::mousePosition.x - FRAME_BUFFER_WIDTH / 2, -(Input::mousePosition.y - FRAME_BUFFER_HEIGHT / 2), Input::mousePosition.z);
		}
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};

