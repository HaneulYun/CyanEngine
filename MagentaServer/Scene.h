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

public:
	static Scene* scene;

public:
	Scene();
	virtual ~Scene();

	virtual void Start();
	virtual void Update();

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	GameObject* CreateGameObject();
	GameObject* CreateGameObject(GameObject*);
	//GameObject* AddGameObject();
	//GameObject* AddGameObject(GameObject* gameObject);
	//void RemoveGameObject(GameObject* gameObject);

	GameObject* CreateEmpty();
	GameObject* Duplicate(GameObject*);
	void Delete(GameObject* gameObject);
};