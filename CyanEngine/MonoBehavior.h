#pragma once
#include "framework.h"

template <typename T>
class MonoBehavior : public Component
{
private:
	virtual Component* Duplicate() { return new T; };
	virtual Component* Duplicate(Component* component) { return new T(*(T*)component); }
};

