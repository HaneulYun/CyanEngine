#pragma once
#include "framework.h"

class Controller : public MonoBehavior<Controller>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:
	float speed{ 30.f };
	GameObject* gameObject;

private:
	friend class GameObject;
	friend class MonoBehavior<Controller>;
	Controller() = default;
	Controller(Controller&) = default;

public:
	~Controller() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
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

		if (Input::GetKey(KeyCode::Q))
			gameObject->transform->Rotate({ 0, 1, 0 }, -speed * Time::deltaTime);
		if (Input::GetKey(KeyCode::E))
			gameObject->transform->Rotate({ 0, 1, 0 }, speed * Time::deltaTime);
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

