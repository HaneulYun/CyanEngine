#include "pch.h"
#include "SceneManager.h"

std::unordered_map<std::string, Scene*> SceneManager::scenes;
Scene* SceneManager::scene{ nullptr };
Scene* SceneManager::nextScene{ nullptr };

void SceneManager::LoadScene(std::string name)
{
	nextScene = scenes[name];
}
