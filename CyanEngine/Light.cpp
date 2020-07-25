#include "pch.h"
#include "Light.h"

void Light::Start()
{
	gameObject->layer = (int)RenderLayer::Light;
	gameObject->scene->objectRenderManager.AddGameObject(gameObject, gameObject->layer);
	gameObject->scene->lightResourceManager.AddGameObject(gameObject, type);
}
