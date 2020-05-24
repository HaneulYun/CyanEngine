#include "pch.h"
#include "Terrain.h"

void Terrain::Start()
{
	gameObject->scene->objectRenderManager.AddGameObject(gameObject, gameObject->layer);
}

void Terrain::Set()
{
	terrainData.Load();
}