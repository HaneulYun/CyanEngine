#pragma once

class QuadTree
{
public:
	QuadNode* root;

	int width{ 1000 };
	int height{ 1000 };

	int nodeCapacity{ 100 };

public:
	void AddGameObject(GameObject* gameObject);
};
