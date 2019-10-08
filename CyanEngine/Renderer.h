#pragma once

class RendererManager;

class Renderer : public Component
{
private:
	RendererManager* rendererManager;
	std::deque<Material*> materials;

public:
	Renderer();
	~Renderer();

	void Start() override;

	void Update() override;
	void Render() override;

	void Destroy() override;
};