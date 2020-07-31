#pragma once

class SpatialPartitioningManager
{
public:
	TagData tagData;
public:
	float worldXMin{ 0 };
	float worldXMax{ 1000 };
	float worldYMin{ 0 };
	float worldYMax{ 1000 };

	int sectorWidth{ 20 };
	int sectorHeight{ 20 };

	int xSize;
	int ySize;

	std::vector<std::vector<Sector>> sectorList;

public:
	void Update();

	void InitSector(float xMin = 0, float xMax = 1000, float yMin = 0, float yMax = 1000, int width = 20, int height = 20);

	void AddGameObject(GameObject* gameObject);

	void DeleteGameObject(GameObject* gameObject);

	bool Contain(int x, int y, GameObject* gameObject);

};
