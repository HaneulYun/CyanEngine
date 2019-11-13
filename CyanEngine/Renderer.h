#pragma once

class RendererManager;

class Renderer : public Component
{
private:
	RendererManager* rendererManager{ nullptr };

public:
	//std::deque<Material*> materials;
	Material* material{ nullptr };

private:
	friend class GameObject;
	Renderer();

public:
	~Renderer();

	void Start() override;

	void Update() override;
	void Render() override;

	void Destroy() override;

	virtual Component* Duplicate() { return new Renderer; };
};