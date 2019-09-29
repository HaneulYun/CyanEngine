#pragma once

class Scene
{
private:
	std::deque<GameObject*> gameObjects;
	Component* renderer;

public:
	Scene();
	~Scene();

	void OnStart();

	void Update();
	void Render();

	void OnDestroy();

	//----------------//
	void BuildObjects();
	void ReleaseObjects();
	void AnimateObjects();
};

