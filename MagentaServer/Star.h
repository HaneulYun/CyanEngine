#pragma once

class Star
{
private:
	float standardAngle = 0.f;	// 클라의 기준 위치
	// id 0인 클라는 0.f, id 1인 클라는 0.f+120.f, id 2인 클라는 0.f+240.f
	float rotatingSpeed = 30.f;

public:
	void Update();
	float getStandardAngle();
};