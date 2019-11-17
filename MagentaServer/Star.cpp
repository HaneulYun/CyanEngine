#include <stdio.h>
#include "Star.h"
#include "Time.h"

void Star::Update()
{
	static float time = 0;
	time += Time::deltaTime;

	standardAngle += rotatingSpeed * Time::deltaTime;
	printf("Angle: %f\n", standardAngle);
}

float Star::getStandardAngle()
{
	return standardAngle;
}