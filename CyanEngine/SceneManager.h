#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include "framework.h"
//#include "StarGuardian.h"
#include "Message.h"

enum {WAIT, START, END};

class SceneManager : public Component
{
private:

public:
	static GameObject* scenemanager; 
	GameObject* playerprefab;
	GameObject* player[3];
	GameObject* star;
	SOCKET* sock;

	//GameObject* gameObject{ nullptr };
	float speedRotating{ 30.f };
	float angle{ 0.0f };
	int gameState{ WAIT };
	int myid{ 0 };
	bool ready{ false };


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
	}

	void Update()
	{
		static float time = 0;
		time += Time::deltaTime;
		if (gameState == START)
			angle += speedRotating * Time::deltaTime;

		if (Input::GetMouseButtonDown(0) && ready == false) {
			ready = true;
			Message message;
			message.msgId = MESSAGE_READY;
			message.lParam = myid;
			int retval = send(*sock, (char*)& message, sizeof(Message), 0);
		}
	}

	void CreatePlayer(int id) {
		player[id] = playerprefab->scene->Instantiate(playerprefab);
		player[id]->transform->position.xmf3 = XMFLOAT3(25.f * cos(120 * id * PI / 180.0f), 25.f * sin(120 * id * PI / 180.0f), 0.0f);
		/*player[id]->GetComponent<RevolvingBehavior>()->speedRotating = speedRotating;
		player[id]->GetComponent<RevolvingBehavior>()->angle = angle + 120 * id;*/
		XMFLOAT4 color[3] = { XMFLOAT4(1, 1, 0, 1), XMFLOAT4(0, 1, 1, 1), XMFLOAT4(1, 0, 1, 1) };
		player[id]->GetComponent<Renderer>()->material->albedo = color[id];
	}

	void StartGame() {
		gameState = START;
		for (int i = 0; i < 3; ++i) {
			RevolvingBehavior* revolvingBehavior = player[i]->AddComponent<RevolvingBehavior>();
			revolvingBehavior->target = star;
			revolvingBehavior->radius = 25.f;
			revolvingBehavior->angle = 120.f * i;
			//player[i]->GetComponent<RevolvingBehavior>()->angle = angle + 120 * i;
		}
	}

};

