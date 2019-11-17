#pragma once
#include "framework.h"
#include "StarGuardian.h"

enum {Wait, Start, End};

class SceneManager : public Component
{
private:

public:
	static GameObject* playerprefab;
	static GameObject* player[3];
	static GameObject* scenemanager;

	//GameObject* gameObject{ nullptr };
	float speedRotating{ 30.f };
	float angle{ 0.0f };
	int gameState{ Wait };
	int myid{ 0 };


private:
	friend class GameObject;
	SceneManager() = default;
	SceneManager(SceneManager&) = default;

public:
	~SceneManager() {}
	virtual Component* Duplicate() { return new SceneManager; }
	virtual Component* Duplicate(Component* component) { return new SceneManager(*(SceneManager*)component); }

	void Start()
	{
		scenemanager = gameObject;
		//int xObjects = 1, yObjects = 1, zObjects = 1, i = 0;
		//for (int x = -xObjects; x <= xObjects; x++)
		//	for (int y = -yObjects; y <= yObjects; y++)
		//		for (int z = -zObjects; z <= zObjects; z++)
		//		{
		//			GameObject* instance = Instantiate(gameObject);
		//
		//			instance->transform->position = XMFLOAT3{ 10.0f * x, 10.0f * y, 10.0f * z };
		//			//instance->GetComponent<Transform>()->position = XMFLOAT3{ 10.0f * x, 10.0f * y, 10.0f * z };
		//			
		//			instance->GetComponent<RotatingBehavior>()->speedRotating = 0;// 10.0f * (i++ % 10);
		//		}
	}

	void Update()
	{
		static float time = 0;
		time += Time::deltaTime;

		angle += speedRotating * Time::deltaTime;
	}

	bool isReady(int num)
	{
		return player[num]->GetComponent<StarGuadian>()->ready;
	}

	void CreatePlayer(int id) {
		player[id] = playerprefab->scene->Instantiate(playerprefab);
		player[id]->GetComponent<RevolvingBehavior>()->speedRotating = speedRotating;
		player[id]->GetComponent<RevolvingBehavior>()->angle = angle + 120 * id;
		XMFLOAT4 color[3] = { XMFLOAT4(1, 0, 0, 1), XMFLOAT4(0, 1, 0, 1), XMFLOAT4(0, 0, 1, 1) };
		player[id]->GetComponent<Renderer>()->material->albedo = color[id];
	}
};

