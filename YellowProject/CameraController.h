#pragma once
#include "..\CyanEngine\framework.h"

class CameraController : public MonoBehavior<CameraController>
{
private /*�� ������ private ������ �����ϼ���.*/:
	float mTheta = 1.5f * XM_PI;
	float mPhi = 0.2f * XM_PI;
	float mRadius = 15.0f;

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
		float x = mRadius * sinf(mPhi) * cosf(mTheta);
		float z = mRadius * sinf(mPhi) * sinf(mTheta);
		float y = mRadius * cosf(mPhi);

		gameObject->GetComponent<Camera>()->pos = XMFLOAT3(x, y, z);

		if (Input::GetMouseButtonDown(0))
		{
			lastMousePos = Input::mousePosition;
		}
		else if (Input::GetMouseButton(0))
		{
			Vector3 currMousePos = Input::mousePosition;
			Vector3 delta = (currMousePos - lastMousePos) * 0.25f * (XM_PI / 180.0f);

			mTheta += delta.x;
			mPhi += delta.y;

			//mPhi = std::(mPhi, 0.1f, PI - 0.1f);

			lastMousePos = currMousePos;
		}
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};