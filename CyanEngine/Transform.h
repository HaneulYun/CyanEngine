#pragma once

class Transform : public Component
{
public:
	XMFLOAT3 localPosition;
	XMFLOAT3 eulerAngles;

	Vector3 localScale{ 1, 1, 1 };

	union
	{
		XMFLOAT4X4 localToWorldMatrix = MathHelper::Identity4x4();
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

	virtual Component* Duplicate() { return new Transform; };
	virtual Component* Duplicate(Component* component) { return new Transform(*(Transform*)component); }

	void Rotate(const XMFLOAT3& axis, float angle)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&axis), XMConvertToRadians(angle));
		localToWorldMatrix = NS_Matrix4x4::Multiply(mtxRotate, localToWorldMatrix);
	}
	void Scale(const XMFLOAT3& scale)
	{
		XMMATRIX mtxRotate = XMMatrixScalingFromVector(XMLoadFloat3(&scale));
		localToWorldMatrix = NS_Matrix4x4::Multiply(mtxRotate, localToWorldMatrix);
	}
};

