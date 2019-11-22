#pragma once

class Star : public Component
{
private:
	float standardAngle = 0.f;	// 클라의 기준 위치
	// id 0인 클라는 0.f, id 1인 클라는 0.f+120.f, id 2인 클라는 0.f+240.f
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
		// 초기화 코드를 작성하세요.
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