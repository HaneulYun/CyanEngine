#pragma once
enum { WAIT, START, END };

class ObjectIDManager;
class BulletGenerator;

class SceneManager : public MonoBehavior<SceneManager>
{
private:

public:
	static SceneManager* scenemanager;
	ObjectIDManager* objectIDmanager;
	BulletGenerator* bulletGenerator;

	GameObject* player[3];
	GameObject* star;
	GameObject* enemyprefab[5]{ nullptr };
	GameObject* bulletprefab[5]{ nullptr };

	//GameObject* gameObject{ nullptr };
	float standardAngle{ 0.f };// 클라의 기준 위치
	// id 0인 클라는 0.f, id 1인 클라는 0.f+120.f, id 2인 클라는 0.f+240.f

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

	void Start();

	void Update();

	void CreatePlayer(int id);

	void StartGame();

	float getStandardAngle();

	int CreateBulletAndGetObjID(int type);

	void DeleteObjectID(int id);

	bool AddDamageToEnemy(int enemyID, int amount);
};