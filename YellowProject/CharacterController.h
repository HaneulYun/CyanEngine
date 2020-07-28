#pragma once
#include <iostream>
#include "..\CyanEngine\framework.h"

class CharacterController : public MonoBehavior<CharacterController>
{
private /*�� ������ private ������ �����ϼ���.*/:
	Animator* anim{ nullptr };

public  /*�� ������ public ������ �����ϼ���.*/:
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
	GameObject* root{ nullptr };

private:
	friend class GameObject;
	friend class MonoBehavior<CharacterController>;
	CharacterController() = default;
	CharacterController(CharacterController&) = default;

public:
	~CharacterController() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
		anim = gameObject->GetComponent<Animator>();
		ball_l = gameObject->GetChildWithName("pelvis");
		root = gameObject->GetChildWithName("root");
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		if (isPlayer)
		{
			Vector3 position = ball_l->GetMatrix().position;
			Debug::Log((std::to_string(position.x) + " " + std::to_string(position.y) + " " + std::to_string(position.z) + "\n").c_str());
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

		gameObject->transform->position += gameObject->transform->forward * speed * Time::deltaTime;
		gameObject->transform->position += gameObject->transform->right * hori_speed * Time::deltaTime;
		anim->SetFloat("VelocityX", speed);
		anim->SetFloat("VelocityY", hori_speed);

		//gameObject->transform->position.y = terrainData->GetHeight(gameObject->transform->position.x, gameObject->transform->position.z);
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};