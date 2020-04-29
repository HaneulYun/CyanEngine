#include "pch.h"
#include "SkinnedMeshRenderer.h"

void SkinnedMeshRenderer::Start()
{
	gameObject->scene->renderObjectsLayer[(int)RenderLayer::SkinnedOpaque][mesh].gameObjects.push_back(gameObject);
}
