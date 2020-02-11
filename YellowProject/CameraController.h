#pragma once
#include "..\CyanEngine\framework.h"

class CameraController : public MonoBehavior<CameraController>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	Vector3 lastMousePos;

public  /*이 영역에 public 변수를 선언하세요.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<CameraController>;
	CameraController() = default;
	CameraController(CameraController&) = default;

public:
	~CameraController() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
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

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};
