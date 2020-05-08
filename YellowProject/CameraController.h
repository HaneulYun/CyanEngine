#pragma once
#include "..\CyanEngine\framework.h"

class CameraController : public MonoBehavior<CameraController>
{
private /*�� ������ private ������ �����ϼ���.*/:
	//float mTheta = 1.5f * XM_PI;
	//float mPhi = 0.2f * XM_PI;
	//float mRadius = 15.0f;
	//
	Vector3 lastMousePos;

public  /*�� ������ public ������ �����ϼ���.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<CameraController>;
	CameraController() = default;
	CameraController(CameraController&) = default;

public:
	~CameraController() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		if (Input::GetMouseButtonDown(2))
		{
			lastMousePos = Input::mousePosition;
		}
		else if (Input::GetMouseButton(2))
		{
			Vector3 currMousePos = Input::mousePosition;
			Vector3 delta = (currMousePos - lastMousePos) * 0.25f * (XM_PI / 180.0f);

			{
				Matrix4x4 R = Matrix4x4::RotationAxis(gameObject->transform->right, delta.y);
				gameObject->transform->up = gameObject->transform->up.TransformNormal(R);
				gameObject->transform->forward = gameObject->transform->forward.TransformNormal(R);
			}

			{
				Matrix4x4 R = Matrix4x4::RotationY(delta.x);
				gameObject->transform->right = gameObject->transform->right.TransformNormal(R);
				gameObject->transform->up = gameObject->transform->up.TransformNormal(R);
				gameObject->transform->forward = gameObject->transform->forward.TransformNormal(R);
			}
		
			lastMousePos = currMousePos;
		}

		float speed = 10.0f;
		if (Input::GetKey(KeyCode::Shift))
			speed *= 5;
		
		if (Input::GetKey(KeyCode::W))
			gameObject->transform->position += gameObject->transform->forward * speed * Time::deltaTime;
		if (Input::GetKey(KeyCode::S))
			gameObject->transform->position -= gameObject->transform->forward * speed * Time::deltaTime;
		if (Input::GetKey(KeyCode::D))
			gameObject->transform->position += gameObject->transform->right * speed * Time::deltaTime;
		if (Input::GetKey(KeyCode::A))
			gameObject->transform->position -= gameObject->transform->right * speed * Time::deltaTime;
		if (Input::GetKey(KeyCode::R))
			gameObject->transform->position += gameObject->transform->up * speed * Time::deltaTime;
		if (Input::GetKey(KeyCode::F))
			gameObject->transform->position -= gameObject->transform->up * speed * Time::deltaTime;

		if (Input::GetKey(KeyCode::Alpha0))
		{
			gameObject->transform->localToWorldMatrix =
			{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 1, -10, 0
			};
		}
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};
