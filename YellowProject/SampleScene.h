#pragma once
#include "Scene.h"

class SampleScene : public Scene
{
public:
	SampleScene() : Scene() {}
	~SampleScene() {}

	virtual void BuildObjects();
};