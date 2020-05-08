#pragma once
#include "..\CyanEngine\framework.h"

class CameraController : public MonoBehavior<CameraController>
{
private /*�� ������ private ������ �����ϼ���.*/:
	//float mTheta = 1.5f * XM_PI;
	//float mPhi = 0.2f * XM_PI;
	//float mRadius = 15.0f;
	//
	//Vector3 lastMousePos;

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
		float speed = 10.0f;
		
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
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};
