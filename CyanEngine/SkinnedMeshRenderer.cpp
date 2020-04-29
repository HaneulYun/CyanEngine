#include "pch.h"
#include "SkinnedMeshRenderer.h"

void SkinnedMeshRenderer::Start()
{
	gameObject->scene->objectRenderManager.AddGameObject(gameObject, (int)RenderLayer::SkinnedOpaque);
}
