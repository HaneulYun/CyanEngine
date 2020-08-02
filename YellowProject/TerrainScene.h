#pragma once
#include "Scene.h"

class TerrainScene : public Scene
{
public:
	TerrainScene() : Scene() {}
	~TerrainScene()
	{
		int k = 0;
	};

	virtual void BuildObjects();
};