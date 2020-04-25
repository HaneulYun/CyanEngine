#pragma once
#include "Scene.h"

class TerrainScene : public Scene
{
public:
	TerrainScene() : Scene() {}
	~TerrainScene() {}

	virtual void BuildObjects();
};