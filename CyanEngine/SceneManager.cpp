#include "pch.h"
#include "SceneManager.h"

std::unordered_map<std::string, Scene*> SceneManager::scenes;
Scene* SceneManager::scene{ nullptr };
Scene* SceneManager::nextScene{ nullptr };

SceneManager::~SceneManager()
{
	for (auto v : scenes)
		delete v.second;
	scenes.clear();
}

void SceneManager::LoadScene(std::string name)
{
	nextScene = scenes[name];
}
