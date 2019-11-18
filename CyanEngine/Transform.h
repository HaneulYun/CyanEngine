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
			Vector3 right;		float _00;
			Vector3 up;			float _01;
			Vector3 forward;	float _02;
			Vector3 position;	float _03;
		};
		XMFLOAT4X4 localToWorldMatrix ;
	};

public:
	Transform();
	Transform(Transform&) = default;
	~Transform();

	void Start() override {}
	void Update() override {}
	void Render() override {}

	virtual Component* Duplicate() { return new Transform; };
	virtual Component* Duplicate(Component* component) { return new Transform(*(Transform*)component); }

	void Rotate(const XMFLOAT3& axis, float angle);
};

