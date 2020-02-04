#pragma once
#include "Scene.h"

class SampleScene : public Scene
{
public:
	SampleScene() : Scene() {}
	virtual ~SampleScene() {}

	virtual void BuildObjects();
};