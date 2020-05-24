#pragma once

class TerrainData;

class DetailPrototype
{
public:
	Mesh* mesh{ nullptr };
	Material* material{ nullptr };

public:
	void Set(TerrainData* terrainData);
};

