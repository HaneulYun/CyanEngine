#include "pch.h"
#include "RotatingBehavior.h"

RotatingBehavior::RotatingBehavior() {}

RotatingBehavior::~RotatingBehavior() {}

void RotatingBehavior::Start()
{
}

void RotatingBehavior::Update()
{
	gameObject->transform->Rotate(axis, speedRotating * Time::Instance()->GetTimeElapsed());
}

void RotatingBehavior::Render()
{
}

void RotatingBehavior::Destroy()
{
}
