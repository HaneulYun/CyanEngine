#include "pch.h"
#include "SkinnedMeshRenderer.h"

void SkinnedMeshRenderer::Start()
{
	gameObject->scene->objectRenderManager.AddGameObject(gameObject, (int)RenderLayer::SkinnedOpaque);
	rootBone = gameObject->parent->AddChild(mesh->BoneObjects);

	int boneCount = rootBone->GetChildCount();
	for (int i = 0; i < boneCount; ++i)
	{
		int k = i;
		bones.push_back(rootBone->GetChildWithIndex(k)->transform);
	}
}
