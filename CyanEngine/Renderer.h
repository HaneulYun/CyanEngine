#pragma once

class Renderer : public Component
{
private:
	RendererManager* rendererManager;

public:
	Renderer();
	~Renderer();

	void Start() override;

	void Update() override;
	void Render() override;

	void Destroy() override;

	//--------------//
};