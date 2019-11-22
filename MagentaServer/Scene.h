#pragma once

enum GAMESTATE { Waiting, Runtime, End };
//
//class Scene : public Singleton<Scene>
//{
//public:
//	static Star *star;
//	static int gameState;
//
//public:
//	Scene();
//	~Scene();
//};

class Scene : public Object
{
protected:
	std::deque<GameObject*> gameObjects;
//	RendererManager* rendererManager;

public:
	Scene();
	virtual ~Scene();

	virtual void Start();
	virtual void Update();
	virtual void Destroy();

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	GameObject* CreateGameObject();
	GameObject* CreateGameObject(GameObject* gameObject);
	GameObject* AddGameObject();
	GameObject* AddGameObject(GameObject* gameObject);
	void RemoveGameObject(GameObject* gameObject);
};