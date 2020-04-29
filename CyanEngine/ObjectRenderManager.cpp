#include "pch.h"
#include "ObjectRenderManager.h"

void RenderSets::Update()
{
	if (!isDirty)
		return;

	isDirty = false;
	int instanceIndex = 0;
	for (auto& gameObject : gameObjects)
		gameObject->instanceIndex = instanceIndex++;
}

void RenderSets::AddGameObject(GameObject* gameObject)
{
	isDirty = true;
	gameObjects.push_back(gameObject);
}

void ObjectRenderManager::Update()
{
	if (!isDirty)
		return;

	isDirty = false;
	for (auto& RenderLayer : renderObjectsLayer)
		for (auto& renderSets : RenderLayer)
			renderSets.second.Update();
}

void ObjectRenderManager::AddGameObject(GameObject* gameObject, int layer)
{
	if (!gameObject)
		return;

	Mesh* mesh = gameObject->GetComponent<MeshFilter>()->mesh;
	if (!mesh)
		gameObject->GetComponent<SkinnedMeshRenderer>()->mesh;
	if (!mesh)
		return;

	isDirty = true;
	gameObject->layer = layer;
	renderObjectsLayer[layer][mesh].AddGameObject(gameObject);
}
