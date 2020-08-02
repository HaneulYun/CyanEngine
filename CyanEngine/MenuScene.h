#pragma once
#include "Scene.h"

class MenuScene : public Scene
{
public:
	MenuScene() : Scene() {}
	~MenuScene() {}

	virtual void BuildObjects();
};