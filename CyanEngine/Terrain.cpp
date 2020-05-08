#include "pch.h"
#include "Terrain.h"

void Terrain::Start()
{
	gameObject->scene->objectRenderManager.AddGameObject(gameObject, gameObject->layer);
}

void Terrain::Set()
{
	terrainData.Load();
	mesh = new RenderTexture(0, 0, 257, 257, { 1, 1, 1 }, { 1, 1, 0, 1 }, &terrainData);
}