#include "pch.h"
#include "MeshFilter.h"

void MeshFilter::Start()
{
	gameObject->scene->objectRenderManager.AddGameObject(gameObject, gameObject->layer);
}
