#pragma once

class Star
{
private:
	float standardAngle = 0.f;	// Ŭ���� ���� ��ġ
	// id 0�� Ŭ��� 0.f, id 1�� Ŭ��� 0.f+120.f, id 2�� Ŭ��� 0.f+240.f
	float rotatingSpeed = 30.f;

public:
	void Update();
	float getStandardAngle();
};