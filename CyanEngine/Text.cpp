#include "pch.h"
#include "Text.h"

void Text::Start()
{
	Scene::scene->textObjects.push_back(gameObject);
}

void Text::OnDestroy()
{
	auto iter = find(Scene::scene->textObjects.begin(), Scene::scene->textObjects.end(), gameObject);

	if (iter != Scene::scene->textObjects.end())
		Scene::scene->textObjects.erase(iter);
}