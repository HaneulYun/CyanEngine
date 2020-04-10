#pragma once

class Graphics;

class Renderer : public Component
{
private:
	Graphics* graphics{ nullptr };

public:
	//std::deque<Material*> materials;
	Material* material{ nullptr };

private:
	friend class GameObject;
	Renderer();
	Renderer(Renderer& component)
		: graphics(component.graphics)
	{
		if (component.material)
		{
			material = new Material(*component.material);
		}
	}

public:
	~Renderer();

	void Start() override;
	void Update() override;

	virtual Component* Duplicate() { return new Renderer; };
	virtual Component* Duplicate(Component* component) { return new Renderer(*(Renderer*)component); }
};