#pragma once
#include "..\CyanEngine\framework.h"

class CameraController : public MonoBehavior<CameraController>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	float mTheta = 1.5f * XM_PI;
	float mPhi = 0.2f * XM_PI;
	float mRadius = 15.0f;

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
		float x = mRadius * sinf(mPhi) * cosf(mTheta);
		float z = mRadius * sinf(mPhi) * sinf(mTheta);
		float y = mRadius * cosf(mPhi);

		gameObject->GetComponent<Transform>()->position = { x, y, z };
		gameObject->GetComponent<Transform>()->forward = (Vector3(0, 0, 0) - Vector3(x, y, z)).Normalize();

		if (Input::GetMouseButtonDown(2))
		{
			lastMousePos = Input::mousePosition;
		}
		else if (Input::GetMouseButton(2))
		{
			Vector3 currMousePos = Input::mousePosition;
			Vector3 delta = (currMousePos - lastMousePos) * 0.25f * (XM_PI / 180.0f);

			mTheta += delta.x;
			mPhi += delta.y;

			mPhi = FbxClamp(mPhi, 0.1f, PI - 0.1f);

			lastMousePos = currMousePos;
		}
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};
