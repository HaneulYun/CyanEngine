#include "pch.h"
#include "Light.h"

void Light::Start()
{
	gameObject->layer = (int)RenderLayer::Light;
	gameObject->scene->objectRenderManager.AddGameObject(gameObject, gameObject->layer);
	gameObject->scene->lightResourceManager.AddGameObject(gameObject, type);
}

void Light::OnDestroy()
{
	if (!gameObject->scene)
		return;
	auto& lightObjects = gameObject->scene->lightResourceManager.lightObjects[type];

	if (auto iter = std::find(lightObjects.begin(), lightObjects.end(), lightData); iter != lightObjects.end())
		lightObjects.erase(iter);
}