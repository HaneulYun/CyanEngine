#pragma once
#include "Scene.h"

class GameScene : public Scene
{
public:
	GameScene() : Scene() {}
	virtual ~GameScene() {}

	virtual void BuildObjects();
};

