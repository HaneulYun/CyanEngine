#pragma once
#include "Scene.h"

class GraphicsScene : public Scene
{
public:
	GraphicsScene() : Scene() {}
	~GraphicsScene() {}

	virtual void BuildObjects();
};