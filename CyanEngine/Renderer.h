#pragma once

class Renderer : public MonoBehavior<Renderer>
{
public:
	std::deque<Material*> materials;

protected:
	friend class GameObject;
	friend class MonoBehavior<Renderer>;
	Renderer() = default;
	Renderer(Renderer&) = default;

public:
	~Renderer() {}
};