#pragma once

class SpatialPartitioningManager
{
	QuadTree* tree;

public:
	void AddGameObject(GameObject* gameObject);
};
