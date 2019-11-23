#include "pch.h"
#include "Component.h"

void Component::UpdateComponent()
{
	if (!isStarted)
	{
		isStarted = true;
		this->Start();
	}
	this->Update();
}
