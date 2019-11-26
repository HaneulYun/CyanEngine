#pragma once

class Collider : public Component
{
protected:
	Collider() = default;

public:
	virtual bool Compare(Collider* other) { return false; }
};