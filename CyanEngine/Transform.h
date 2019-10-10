#pragma once

class Transform : public Component
{
public:

	XMFLOAT3 localPosition;
	XMFLOAT3 eulerAngles;

	union
	{
		struct
		{
			XMFLOAT3 right;		float _00;
			XMFLOAT3 up;		float _01;
			XMFLOAT3 forward;	float _02;
			XMFLOAT3 position;	float _03;
		};
		XMFLOAT4X4 localToWorldMatrix ;
	};

public:
	Transform();
	~Transform();

	void Start() override {}
	void Update() override {}
	void Render() override {}
	void Destroy() override {}

	void Rotate(const XMFLOAT3& axis, float angle);
};

