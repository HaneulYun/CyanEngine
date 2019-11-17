#pragma once
#include "Star.h"
#include "Singleton.h"

enum GAMESTATE { Waiting, Runtime, End };

class Scene : public Singleton<Scene>
{
public:
	static Star *star;
	static int gameState;

public:
	Scene();
	~Scene();
};