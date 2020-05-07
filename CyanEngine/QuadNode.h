#pragma once

class QuadTree;

class QuadNode
{
public:
	QuadTree* tree{ nullptr };

	QuadNode* parent{ nullptr };
	QuadNode* child[4]{ nullptr, nullptr, nullptr, nullptr };

	Vector2 point{};

	std::deque<GameObject*> gameObjects;

public:
	void AddGameObject(GameObject* gameObject);
};
