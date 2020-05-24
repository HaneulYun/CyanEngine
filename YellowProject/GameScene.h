#pragma once
#include "Scene.h"

class GameScene : public Scene
{
public:
	GameScene() : Scene() {}
	~GameScene() {}

	virtual void BuildObjects();
};