#pragma once
#include "Scene.h"

class MaterialScene : public Scene
{
public:
	MaterialScene() : Scene() {}
	~MaterialScene() {}

	virtual void BuildObjects();
};