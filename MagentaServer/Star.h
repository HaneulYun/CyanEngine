#pragma once

class Star : public Component
{
private:
	float standardAngle = 0.f;	// Ŭ���� ���� ��ġ
	// id 0�� Ŭ��� 0.f, id 1�� Ŭ��� 0.f+120.f, id 2�� Ŭ��� 0.f+240.f
	float rotatingSpeed = 30.f;

	int health;

private:
	friend class GameObject;
	Star() {}

public:
	~Star() {}
	virtual Component* Duplicate() { return new Star; };

	void Start()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
	}

	void Update()
	{
		static float time = 0.f;
		time += Time::deltaTime;

		standardAngle += rotatingSpeed * Time::deltaTime;
		printf("Angle: %f\n", standardAngle);
	}

	float getStandardAngle()
	{
		return standardAngle;
	}
};