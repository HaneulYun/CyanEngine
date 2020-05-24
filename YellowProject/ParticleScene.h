#pragma once
#include "Scene.h"

class ParticleScene : public Scene
{
public:
	ParticleScene() : Scene() {}
	~ParticleScene() {}

	virtual void BuildObjects();
};
