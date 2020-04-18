#pragma once
#include "..\CyanEngine\framework.h"

class CharacterController : public MonoBehavior<CharacterController>
{
private /*이 영역에 private 변수를 선언하세요.*/:

public  /*이 영역에 public 변수를 선언하세요.*/:
	float speed = 1.0f;

private:
	friend class GameObject;
	friend class MonoBehavior<CharacterController>;
	CharacterController() = default;
	CharacterController(CharacterController&) = default;

public:
	~CharacterController() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		if (Input::GetKey(KeyCode::W))
			gameObject->transform->position += Vector3(0, 0, 1) * speed* Time::deltaTime;
		if (Input::GetKey(KeyCode::S))
			gameObject->transform->position -= Vector3(0, 0, 1) * speed* Time::deltaTime;
		if (Input::GetKey(KeyCode::D))
			gameObject->transform->position += Vector3(1, 0, 0) * speed* Time::deltaTime;
		if (Input::GetKey(KeyCode::A))
			gameObject->transform->position -= Vector3(1, 0, 0) * speed* Time::deltaTime;
		gameObject->NumFramesDirty = NUM_FRAME_RESOURCES;
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};