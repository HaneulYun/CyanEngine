#include "pch.h"
#include "ObjectRenderManager.h"

void RenderSets::Update()
{
	if (!isDirty)
		return;

	isDirty = false;
	int instanceIndex = 0;
	for (auto& gameObject : gameObjects)
	{
		gameObject->instanceIndex = instanceIndex++;
		gameObject->renderSet = this;
	}

	if (!objectsResources.size())
	{
		for (int i = 0; i < NUM_FRAME_RESOURCES; ++i)
		{
			auto resource = std::make_unique<ObjectsResource>();

			int objectCount = gameObjects.size();
			int boneStride = 1;
			int matIndexStride = 1;
			if (gameObjects[0]->GetComponent<Animator>())
				boneStride = gameObjects[0]->GetComponent<Animator>()->controller->BoneCount();
			if (gameObjects[0]->GetComponent<SkinnedMeshRenderer>())
				matIndexStride = gameObjects[0]->GetComponent<SkinnedMeshRenderer>()->materials.size();

			resource->InstanceBuffer = std::make_unique<UploadBuffer<InstanceData>>(Graphics::Instance()->device.Get(), objectCount, false);
			resource->SkinnedBuffer = std::make_unique<UploadBuffer<SkinnnedData>>(Graphics::Instance()->device.Get(), objectCount * boneStride, false);
			resource->MatIndexBuffer = std::make_unique<UploadBuffer<MatIndexData>>(Graphics::Instance()->device.Get(), objectCount * matIndexStride, false);

			objectsResources.push_back(std::move(resource));
		}
	}
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
