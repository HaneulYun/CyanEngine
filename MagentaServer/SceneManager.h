#pragma once
enum { WAIT, START, END };

class SceneManager : public MonoBehavior<SceneManager>
{
private:

public:
	static GameObject* scenemanager;
	GameObject* player[3];
	GameObject* star;

	//GameObject* gameObject{ nullptr };
	float standardAngle{ 0.f };// Ŭ���� ���� ��ġ
	// id 0�� Ŭ��� 0.f, id 1�� Ŭ��� 0.f+120.f, id 2�� Ŭ��� 0.f+240.f

	float speedRotating{ 30.f };
	int gameState{ WAIT };

private:
	friend class GameObject;
	friend class MonoBehavior<SceneManager>;
	SceneManager() = default;
	SceneManager(SceneManager&) = default;

public:
	~SceneManager() {}
	virtual Component* Duplicate() { return new SceneManager; }
	virtual Component* Duplicate(Component* component) { return new SceneManager(*(SceneManager*)component); }

	void Start()
	{
		scenemanager = gameObject;
	}

	void Update()
	{
		static float time = 0.f;
		time += Time::deltaTime;
		if (gameState == START)
			standardAngle += speedRotating * Time::deltaTime;
		if (star->GetComponent<Damageable>()->isDead())
		{
			gameState = END;
			printf("��\n");
		}
	}

	void CreatePlayer(int id) {
		
	}

	void StartGame() {
		gameState = START;
	}

	float getStandardAngle()
	{
		return standardAngle;
	}
};