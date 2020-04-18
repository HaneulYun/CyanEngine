#pragma once
#include "..\CyanEngine\framework.h"

class CharacterController : public MonoBehavior<CharacterController>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	float speed = 1.0f;

private:
	friend class GameObject;
	friend class MonoBehavior<CharacterController>;
	CharacterController() = default;
	CharacterController(CharacterController&) = default;

public:
	~CharacterController() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
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

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};