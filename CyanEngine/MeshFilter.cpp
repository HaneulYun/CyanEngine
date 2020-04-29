#include "pch.h"
#include "MeshFilter.h"

void MeshFilter::Start()
{
	gameObject->scene->objectRenderManager.AddGameObject(gameObject, gameObject->layer);
	gameObject->scene->renderObjectsLayer[gameObject->layer][mesh].gameObjects.push_back(gameObject);
}
