#pragma once

class Graphics;

class Renderer : public Component
{
private:
	Graphics* graphics{ nullptr };

public:
	std::deque<int> materials;

protected:
	friend class GameObject;
	Renderer();
	Renderer(Renderer&) = default;

public:
	~Renderer();

	void Start() override;
	void Update() override;

	virtual Component* Duplicate() { return new Renderer; };
	virtual Component* Duplicate(Component* component) { return new Renderer(*(Renderer*)component); }
};