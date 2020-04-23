#pragma once

class Graphics;

class Renderer : public MonoBehavior<Renderer>
{
private:
	Graphics* graphics{ nullptr };

public:
	std::deque<int> materials;

protected:
	friend class GameObject;
	friend class MonoBehavior<Renderer>;
	Renderer();
	Renderer(Renderer&) = default;

public:
	~Renderer() {}
};
