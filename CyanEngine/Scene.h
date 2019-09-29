#pragma once

class Scene
{
private:
	std::deque<GameObject*> gameObjects;

public:
	Scene();
	~Scene();
};

