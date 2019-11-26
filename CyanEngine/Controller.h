#pragma once
#include "framework.h"

class Controller : public MonoBehavior<Controller>
{
private /*�� ������ private ������ �����ϼ���.*/:

public  /*�� ������ public ������ �����ϼ���.*/:

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
		if (Input::GetKeyDown(KeyCode::W))
			gameObject->transform->position = gameObject->transform->position + gameObject->transform->forward * 1;
		if (Input::GetKeyDown(KeyCode::S))
			gameObject->transform->position = gameObject->transform->position - gameObject->transform->forward * 1;
		if (Input::GetKeyDown(KeyCode::D))
			gameObject->transform->position = gameObject->transform->position + gameObject->transform->right * 1;
		if (Input::GetKeyDown(KeyCode::A))
			gameObject->transform->position = gameObject->transform->position - gameObject->transform->right * 1;
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

