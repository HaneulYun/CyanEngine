#include "pch.h"
#include "Component.h"

Component::Component()
{
}

Component::~Component()
{
}

void Component::UpdateComponent()
{
	if (isStarted)
		return this->Update();
	isStarted = true;
	return this->Start();
}
