#pragma once
#include "framework.h"

template <typename T, typename S = Component>
class MonoBehavior : public S
{
private:
	virtual Component* Duplicate() { return new T; };
	virtual Component* Duplicate(Component* component) { return new T(*(T*)component); }
};