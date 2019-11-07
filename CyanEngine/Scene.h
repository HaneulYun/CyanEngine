#pragma once

class Scene
{
private:
	std::deque<GameObject*> gameObjects;
	RendererManager* rendererManager;

public:
	Scene();
	~Scene();

	void Start();
	void Update();
	void Render();
	void Destroy();

	//----------------//
	void BuildObjects(ID3D12Device* _device = nullptr);
	void ReleaseObjects();
};