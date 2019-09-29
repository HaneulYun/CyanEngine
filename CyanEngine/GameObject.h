#pragma once

class GameObject : public Object
{
private:
	std::deque<Component*> components;

public:
	GameObject();
	~GameObject();
};