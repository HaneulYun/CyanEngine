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
	if (!isStarted)
	{
		isStarted = true;
		this->Start();
	}
	this->Update();
}
