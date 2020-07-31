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

					sector.second.erase(std::find(sector.second.begin(), sector.second.end(), gameObject));
					AddGameObject(gameObject);
				}
			}
		}
	}

	// Collision Check
	Collider* lhs_collider, * rhs_collider;
	for (int x = 0; x < xSize; ++x)
	{
		for (int y = 0; y < ySize; ++y)
		{
			std::vector<Sector*> rightSectorList;
			rightSectorList.push_back(&sectorList[x][y]);
			if (x > 0) rightSectorList.push_back(&sectorList[x - 1][y]);
			if (x < xSize - 1) rightSectorList.push_back(&sectorList[x + 1][y]);
			if (y > 0) rightSectorList.push_back(&sectorList[x][y - 1]);
			if (y < ySize - 1) rightSectorList.push_back(&sectorList[x][y + 1]);
			if (x > 0 && y > 0) rightSectorList.push_back(&sectorList[x - 1][y - 1]);
			if (x > 0 && y < ySize - 1) rightSectorList.push_back(&sectorList[x - 1][y + 1]);
			if (x < xSize - 1 && y > 0) rightSectorList.push_back(&sectorList[x + 1][y - 1]);
			if (x < xSize - 1 && y < ySize - 1) rightSectorList.push_back(&sectorList[x + 1][y + 1]);

			for (auto& leftList : sectorList[x][y].list)
			{
				for (auto& rightSector : rightSectorList)
				{
					for (auto& rightList : rightSector->list)
					{
						if (tagData.GetTagCollision(leftList.first, rightList.first) == false)
							continue;

						for (auto& left : leftList.second)
						{
							for (auto& right : rightList.second)
							{
								if (left == right) continue;
								lhs_collider = left->GetComponent<BoxCollider>();
								rhs_collider = right->GetComponent<BoxCollider>();

								auto iter = left->collisionType.find(right);

								if (lhs_collider->Compare(rhs_collider))
								{
									if (iter == left->collisionType.end())
										(left->collisionType)[right] = CollisionType::eTriggerEnter;
									else if (iter->second == CollisionType::eTriggerEnter)
										iter->second = CollisionType::eTriggerStay;
								}
								else if (iter != left->collisionType.end())
									iter->second = CollisionType::eTriggerExit;
							}
						}
					}
				}
			}
		}
	}


	for (auto& sectorX : sectorList)
	{
		for (auto& sectorXY : sectorX)
		{
			for (auto& gameObjects : sectorXY.list)
			{
				for (auto& gameObject : gameObjects.second)
				{
					for (auto iter = gameObject->collisionType.begin(); iter != gameObject->collisionType.end();)
					{
						switch (iter->second)
						{
						case CollisionType::eCollisionEnter:
							gameObject->OnCollisionEnter(iter->first); break;
						case CollisionType::eCollisionStay:
							gameObject->OnCollisionStay(iter->first); break;
						case CollisionType::eCollisionExit:
							gameObject->OnCollisionExit(iter->first); break;
						case CollisionType::eTriggerEnter:
							gameObject->OnTriggerEnter(iter->first); break;
						case CollisionType::eTriggerStay:
							gameObject->OnTriggerStay(iter->first); break;
						}
						if (iter->second == CollisionType::eTriggerExit)
						{
							gameObject->OnTriggerExit(iter->first);
							iter = gameObject->collisionType.erase(iter);
						}
						else
							++iter;
					}
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
	int x = (int)gameObject->transform->position.x / sectorWidth;
	int y = (int)gameObject->transform->position.z / sectorHeight;
	if (0 > x || x > xSize || 0 > y || y > ySize) return;
	sectorList[x][y].list[gameObject->tag].push_back(gameObject);
}

void SpatialPartitioningManager::DeleteGameObject(GameObject* gameObject)
{
	if (gameObject->GetComponent<BoxCollider>() == nullptr) return;
	int x = (int)gameObject->transform->position.x / sectorWidth;
	int y = (int)gameObject->transform->position.z / sectorHeight;

	sectorList[x][y].list[gameObject->tag].erase(std::find(sectorList[x][y].list[gameObject->tag].begin(), sectorList[x][y].list[gameObject->tag].end(), gameObject));
}

bool SpatialPartitioningManager::Contain(int x, int y, GameObject* gameObject)
{
	if (sectorWidth * x <= gameObject->transform->position.x && gameObject->transform->position.x <= sectorWidth * (x + 1))
		if (sectorHeight * y <= gameObject->transform->position.z && gameObject->transform->position.z <= sectorHeight * (y + 1))
			return true;
	return false;
}