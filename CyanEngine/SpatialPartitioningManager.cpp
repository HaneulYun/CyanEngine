#include "pch.h"
#include "SpatialPartitioningManager.h"

void SpatialPartitioningManager::AddGameObject(GameObject* gameObject)
{
	tree->AddGameObject(gameObject);
}
