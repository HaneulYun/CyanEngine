#include "Scene.h"

Star* Scene::star;
int Scene::gameState;

Scene::Scene()
{
	star = new Star();
	gameState = Waiting;
}

Scene::~Scene()
{
	delete star;
}
