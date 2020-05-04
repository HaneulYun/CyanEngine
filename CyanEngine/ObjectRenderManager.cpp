#include "pch.h"
#include "ObjectRenderManager.h"

std::unique_ptr<ObjectsResource> RenderSets::MakeResource()
{
	auto resource = std::make_unique<ObjectsResource>();

	int objectCount = gameObjects.size();
	int boneStride = 1;
	int matIndexStride = 1;
	if (gameObjects[0]->GetComponent<Animator>())
		boneStride = gameObjects[0]->GetComponent<Animator>()->controller->BoneCount();
	if (gameObjects[0]->GetComponent<Renderer>())
		matIndexStride = gameObjects[0]->GetComponent<Renderer>()->materials.size();
	else if (gameObjects[0]->GetComponent<SkinnedMeshRenderer>())
		matIndexStride = gameObjects[0]->GetComponent<SkinnedMeshRenderer>()->materials.size();

	resource->InstanceBuffer = std::make_unique<UploadBuffer<InstanceData>>(Graphics::Instance()->device.Get(), objectCount, false);
	resource->SkinnedBuffer = std::make_unique<UploadBuffer<SkinnnedData>>(Graphics::Instance()->device.Get(), objectCount * boneStride, false);
	resource->MatIndexBuffer = std::make_unique<UploadBuffer<MatIndexData>>(Graphics::Instance()->device.Get(), objectCount * matIndexStride, false);
	
	return resource;
}

void RenderSets::Update()
{
	if (!isDirty)
		return;

	--isDirty;
	activatedInstance = 0;
	for (auto& gameObject : gameObjects)
	{
		gameObject->instanceIndex = gameObject->active ? activatedInstance++ : -1;
		gameObject->renderSet = this;
	}
	if (!activatedInstance)
		return;

	if (!objectsResources.size())
	{
		for (int i = 0; i < NUM_FRAME_RESOURCES; ++i)
		{
			objectsResources.push_back(MakeResource());
		}
	}
	else
	{
		int index = Scene::scene->frameResourceManager.currFrameResourceIndex;
		objectsResources[index].reset();
		objectsResources[index] = MakeResource();
	}
}

void RenderSets::AddGameObject(GameObject* gameObject)
{
	isDirty = NUM_FRAME_RESOURCES;
	gameObjects.push_back(gameObject);
}

ObjectsResource* RenderSets::GetResources()
{
	int index = Scene::scene->frameResourceManager.currFrameResourceIndex;
	return objectsResources[index].get();
}

void ObjectRenderManager::Update()
{
	for (auto& RenderLayer : renderObjectsLayer)
		for (auto& renderSets : RenderLayer)
			renderSets.second.Update();
}

void ObjectRenderManager::AddGameObject(GameObject* gameObject, int layer)
{
	if (!gameObject)
		return;

	Mesh* mesh{ nullptr };
	if (auto component = gameObject->GetComponent<MeshFilter>(); component)
		mesh = component->mesh;
	if (auto component = gameObject->GetComponent<SkinnedMeshRenderer>(); component && !mesh)
		mesh = component->mesh;
	if (!mesh)
		return;

	gameObject->layer = layer;
	renderObjectsLayer[layer][mesh].AddGameObject(gameObject);
}
