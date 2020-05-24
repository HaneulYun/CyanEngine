#pragma once

class SceneManager : public Singleton<SceneManager>
{
public:
	static std::unordered_map<std::string, Scene*> scenes;
	static Scene* scene;
	static Scene* nextScene;

	static void LoadScene(std::string name);
};
