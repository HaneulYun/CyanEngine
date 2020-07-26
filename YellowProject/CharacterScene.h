#pragma once
#include "Scene.h"

class CharacterScene : public Scene
{
public:
	CharacterScene() : Scene() {}
	~CharacterScene() {}

	virtual void BuildObjects();
};