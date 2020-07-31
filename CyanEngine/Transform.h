#pragma once

class Transform : public Component
{
public:
	XMFLOAT3 localPosition;
	XMFLOAT3 eulerAngles;

	Vector3 localScale{ 1, 1, 1 };

	union
	{
		Matrix4x4 localToWorldMatrix = Matrix4x4::MatrixIdentity();
		struct
		{
			Vector3 right;		float _00;
			Vector3 up;			float _01;
			Vector3 forward;	float _02;
			Vector3 position;	float _03;
		};
	};

public:
	Transform() {};
	Transform(Transform&) = default;
	~Transform() {};

	void Start() override {}
	void Update() override {}
	void OnDestroy();

	virtual Component* Duplicate() { return new Transform; };
	virtual Component* Duplicate(Component* component) { return new Transform(*(Transform*)component); }

	void Rotate(const Vector3& axis, float angle/*degree*/)
	{
		Matrix4x4 mtxRotate = Matrix4x4::RotationAxis(axis, XMConvertToRadians(angle));
		localToWorldMatrix = mtxRotate * localToWorldMatrix;
	}
	void Scale(const Vector3& scale)
	{
		Matrix4x4 mtxScale = Matrix4x4::ScalingFromVector(scale);
		localToWorldMatrix = mtxScale * localToWorldMatrix;
	}
};

