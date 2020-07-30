#include "pch.h"
#include "SpatialPartitioningManager.h"

void SpatialPartitioningManager::Update()
{
	// SectorList Update
	for (int x = 0; x < xSize; ++x)
	{
		for (int y = 0; y < ySize; ++y)
		{
			for (auto& sector : sectorList[x][y].list)
			{
				for (auto& gameObject : sector.second)
				{
					if (Contain(x, y, gameObject)) continue;

				}
			}

		}
	}

	// Collision Check
	for (auto& sectorX : sectorList)
	{
		for (auto& sectorXY : sectorX)
		{
			for (auto& left : sectorXY.list)
			{
				for (auto& right : sectorXY.list)
				{
					if (right.first < left.first)
						continue;
					if (tagData.GetTagCollision(left.first, right.first) == false)
						continue;


				}
			}
		}
	}
}

void SpatialPartitioningManager::InitSector(float xMin, float xMax, float yMin, float yMax, int width, int height)
{
	worldXMin = xMin;
	worldXMax = xMax;
	worldYMin = yMin;
	worldYMax = yMax;
	sectorWidth = width;
	sectorHeight = height;

	xSize = (worldXMax - worldXMin) / sectorWidth;
	ySize = (worldYMax - worldYMin) / sectorHeight;

	sectorList.assign(xSize, std::vector<Sector>(ySize, Sector()));

	tagData.AddTag("Default");
}

void SpatialPartitioningManager::AddGameObject(GameObject* gameObject)
{
	int x = (int)gameObject->transform->position.x % sectorWidth;
	int y = (int)gameObject->transform->position.y % sectorHeight;

	sectorList[x][y].list[gameObject->tag].push_back(gameObject);
}

bool SpatialPartitioningManager::Contain(int x, int y, GameObject* gameObject)
{
	if (sectorWidth * x <= gameObject->transform->position.x && gameObject->transform->position.x <= (sectorWidth + 1) * x)
		if (sectorHeight * x <= gameObject->transform->position.y && gameObject->transform->position.y <= (sectorHeight + 1) * x)
			return true;
	return false;
}