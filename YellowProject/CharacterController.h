#pragma once
#include <iostream>
#include "..\CyanEngine\framework.h"

class CharacterController : public MonoBehavior<CharacterController>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	Animator* anim{ nullptr };

public  /*이 영역에 public 변수를 선언하세요.*/:
	float speed = 0.0f;
	float hori_speed = 0.0f;

private:
	friend class GameObject;
	friend class MonoBehavior<CharacterController>;
	CharacterController() = default;
	CharacterController(CharacterController&) = default;

public:
	~CharacterController() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
		anim = gameObject->GetComponent<Animator>();
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		if (Input::GetKey(KeyCode::W))
		{
			if (speed < 4.0f)
			{
				speed += 1.0f * Time::deltaTime;
				if (speed > 1.0f)
					speed = 1.0f;
			}
		}
		else
		{
			if (speed > 0.0f)
			{
				speed -= 2.0f * Time::deltaTime;
				if (speed < 0.0f)
					speed = 0.0f;
			}
		}
		
		if (Input::GetKey(KeyCode::S))
		{
			if (speed > -4.0f)
			{
				speed -= 1.0f * Time::deltaTime;
				if (speed < -1.0f)
					speed = -1.0f;
			}
		}
		else
		{
			if (speed < 0.0f)
			{
				speed += 2.0f * Time::deltaTime;
				if (speed > 0.0f)
					speed = 0.0f;
			}
		}
		
		if (Input::GetKey(KeyCode::D))
		{
			if (hori_speed < 4.0f)
			{
				hori_speed += 1.0f * Time::deltaTime;
				if (hori_speed > 1.0f)
					hori_speed = 1.0f;
			}
		}
		else
		{
			if (hori_speed > 0.0f)
			{
				hori_speed -= 4.0f * Time::deltaTime;
				if (hori_speed < 0.0f)
					hori_speed = 0.0f;
			}
		}
		
		if (Input::GetKey(KeyCode::A))
		{
			if (hori_speed > -3.0f)
			{
				hori_speed -= 1.0f * Time::deltaTime;
				if (hori_speed < -1.0f)
					hori_speed = -1.0f;
			}
		}
		else
		{
			if (hori_speed < 0.0f)
			{
				hori_speed += 2.0f * Time::deltaTime;
				if (hori_speed > 0.0f)
					hori_speed = 0.0f;
			}
		}

		gameObject->transform->position += Vector3(hori_speed, 0, speed) * Time::deltaTime;
		anim->SetFloat("Speed", speed);
		anim->SetFloat("HoriSpeed", hori_speed);

		gameObject->NumFramesDirty = NUM_FRAME_RESOURCES;
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
	void test()
	{
		Debug::Log(" zzzzzz 되겠냨ㅋ\n");
	}
};