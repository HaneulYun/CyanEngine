#pragma once

class Renderer : public Component,
	public Singleton<Renderer, Component>
{
public:
	Renderer();
	~Renderer();

	void OnStart();

	void Update();
	void Render();

	void OnDestroy();
};