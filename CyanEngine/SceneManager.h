#pragma once

class SceneManager : public Singleton<SceneManager>
{
public:
	std::unordered_map<std::string, Scene*> scenes;
	Scene* scene;
};
