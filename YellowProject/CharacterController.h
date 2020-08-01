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

	float mTheta = 0.0f * XM_PI;
	float mPhi = 0.0f * XM_PI;
	float mRadius = 5.0f;
	Vector3 lookAtPos = { 0,0,0 };

	Vector3 lastMousePos;

	bool isPlayer{ true };

	TerrainData* terrainData{ nullptr };

	AudioSource* audioSource{ nullptr };

	GameObject* ball_l{ nullptr };
	GameObject* ball_r{ nullptr };
	GameObject* root{ nullptr };

	enum FootState { OnGround, ToAir, OnAir, ToGround };

	FootState leftFootState{ OnGround };
	FootState rightFootState{ OnGround };

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
		ball_l = gameObject->GetChildWithName("ball_l");
		ball_r = gameObject->GetChildWithName("ball_r");
		root = gameObject->GetChildWithName("root");
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		if (isPlayer)
		{
			Vector3 leftFootPosition = (ball_r->GetMatrix(root) * Matrix4x4::RotationX(-3.141592 * 0.5)).position;
			Vector3 rightFootPosition = (ball_l->GetMatrix(root) * Matrix4x4::RotationX(-3.141592 * 0.5)).position;

			switch (leftFootState)
			{
			case CharacterController::OnGround:
				if (leftFootPosition.y > 0.03) leftFootState = ToAir;
				break;
			case CharacterController::ToAir:
				leftFootState = OnAir;
				audioSource->Play();
				break;
			case CharacterController::OnAir:
				if (leftFootPosition.y < 0.03) leftFootState = ToGround;
				break;
			case CharacterController::ToGround:
				leftFootState = OnGround;
				break;
			}
			
			switch (rightFootState)
			{
			case CharacterController::OnGround:
				if (rightFootPosition.y > 0.03) rightFootState = ToAir;
				break;
			case CharacterController::ToAir:
				rightFootState = OnAir;
				audioSource->Play();
				break;
			case CharacterController::OnAir:
				if (rightFootPosition.y < 0.03) rightFootState = ToGround;
				break;
			case CharacterController::ToGround:
				rightFootState = OnGround;
				break;
			}

			if (Input::GetKeyDown(KeyCode::R))
			{
				audioSource->Play();
			}
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

			if (Input::GetMouseButtonDown(2))
			{
				lastMousePos = Input::mousePosition;
			}
			else if (Input::GetMouseButton(2))
			{
				Vector3 currMousePos = Input::mousePosition;
				Vector3 delta = (currMousePos - lastMousePos) * 10 * (XM_PI / 180.0f);

				gameObject->transform->Rotate({ 0, 1, 0 }, delta.x);

				lastMousePos = currMousePos;
			}
		}

		gameObject->transform->position += gameObject->transform->forward * speed * Time::deltaTime;
		gameObject->transform->position += gameObject->transform->right * hori_speed * Time::deltaTime;

		anim->SetFloat("VelocityX", hori_speed);
		anim->SetFloat("VelocityZ", speed);
		//gameObject->transform->position.y = terrainData->GetHeight(gameObject->transform->position.x, gameObject->transform->position.z);
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};