#include "pch.h"
#include "Component.h"

void Component::UpdateComponent()
{
	if (isDirty)
	{
		isDirty = false;
		this->Start();
	}
	this->Update();
}
