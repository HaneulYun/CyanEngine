#include "pch.h"
#include "SkinnedMeshRenderer.h"

void SkinnedMeshRenderer::Start()
{
	gameObject->scene->objectRenderManager.AddGameObject(gameObject, (int)RenderLayer::SkinnedOpaque);
	Root();
}

GameObject* SkinnedMeshRenderer::Root()
{
	if (!rootBone)
	{
		auto boneTransform = Scene::scene->DuplicatePrefab(mesh->BoneObjects);
		rootBone = gameObject->parent->AddChild(boneTransform);

		int boneCount = rootBone->GetChildCount();
		for (int i = 0; i < boneCount; ++i)
		{
			int k = i;
			bones.push_back(rootBone->GetChildWithIndex(k)->transform);
		}
	}
	return rootBone;
}