#pragma once

class RendererManager;
class RenderItem;

class Renderer : public Component
{
private:
	RendererManager* rendererManager{ nullptr };

public:
	//std::deque<Material*> materials;
	RenderItem* item{ nullptr };
	Material* material{ nullptr };

private:
	friend class GameObject;
	Renderer();
	Renderer(Renderer& component)
		: rendererManager(component.rendererManager)
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