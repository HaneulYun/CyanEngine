#pragma once
#include "..\CyanEngine\framework.h"

class CameraController : public MonoBehavior<CameraController>
{
private /*�� ������ private ������ �����ϼ���.*/:
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
		if (Input::GetMouseButtonDown(0))
		{
			lastMousePos = Input::mousePosition;
		}
		else if (Input::GetMouseButton(0))
		{
			Vector3 currMousePos = Input::mousePosition;
			Vector3 delta = lastMousePos - currMousePos;

			Camera::main->gameObject->transform->Rotate(XMFLOAT3(0, 1, 0), delta.x);

			lastMousePos = currMousePos;
		}
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};
