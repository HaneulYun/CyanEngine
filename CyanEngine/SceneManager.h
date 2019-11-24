#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include "framework.h"
#include "Message.h"

enum {WAIT, START, END};

class SceneManager : public MonoBehavior<SceneManager>
{
private:

public:
	static GameObject* scenemanager;
	GameObject* playerprefab{ nullptr };
	GameObject* enemyprefab[5]{ nullptr, };
	GameObject* bulletprefab[5]{ nullptr, };
	GameObject* player[3]{ nullptr, };
	GameObject* star{ nullptr };
	SOCKET* sock;

	float speedRotating{ 30.f };
	float angle{ 0.0f };
	int gameState{ WAIT };
	int myid{ 0 };
	int nplayer{ 0 };
	bool ready{ false };
	float spawnRadius{ 200 };


private:
	friend class GameObject;
	friend class MonoBehavior<SceneManager>;
	SceneManager() = default;
	SceneManager(SceneManager&) = default;

public:
	~SceneManager() {}

	Vector3 AngletoDir(float angle)
	{
		Vector3 axis{ 0.0f, 0.0f, 1.0f };
		Vector3 v{ 0.0f, 1.0f, 0.0f };
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&axis.xmf3), XMConvertToRadians(angle));
		return NS_Vector3::TransformCoord(v.xmf3, mtxRotate);
	}

	float DirtoAngle(Vector3 direction) // -180~180
	{
		Vector3 axis{ 0.0f, 1.0f, 0.0f };
		float angle = NS_Vector3::Angle(axis.xmf3, direction.xmf3);

		if (NS_Vector3::CrossProduct(axis.xmf3, direction.xmf3).z < 0.f)
			angle = -angle;

		return angle;
	}

	void StartGame() 
	{
		gameState = START;
		for (int i = 0; i < nplayer; ++i) {
			RevolvingBehavior* revolvingBehavior = player[i]->AddComponent<RevolvingBehavior>();
			revolvingBehavior->target = star;
			revolvingBehavior->radius = 25.f;
			revolvingBehavior->angle = 120.f * i;
			//player[i]->GetComponent<RevolvingBehavior>()->angle = angle + 120 * i;
		}
	}

	void CreatePlayer(int id) 
	{
		++nplayer;
		XMFLOAT4 color[3] = { XMFLOAT4(1, 1, 0, 1), XMFLOAT4(0, 1, 1, 1), XMFLOAT4(1, 0, 1, 1) };

		player[id] = Instantiate(playerprefab);
		player[id]->transform->position.xmf3 = XMFLOAT3(25.f * cos(120 * id * PI / 180.0f), 25.f * sin(120 * id * PI / 180.0f), 0.0f);
		player[id]->GetComponent<Renderer>()->material->albedo = color[id];
		for (int i = 0; i < 5; ++i)
		{
			GameObject* bullet = gameObject->scene->CreateGameObject(bulletprefab[i]);
			bullet->GetComponent<Renderer>()->material->albedo = color[id];
			player[id]->GetComponent<StarGuardian>()->bullet[i] = bullet;

		}
	}

	void CreateBullet(int type, int id, float angle)
	{
		Vector3 direction = AngletoDir(angle);
		player[id]->GetComponent<StarGuardian>()->Shoot(type, direction);
	}

	void CreateEnemy(int type, float radian)
	{
		GameObject* object = Instantiate(enemyprefab[type]);
		{
			object->GetComponent<Transform>()->position = Vector3(cos(radian) * spawnRadius, sin(radian) * spawnRadius, 0);
		}
	}

	void Start()
	{
		scenemanager = gameObject;
	}

	void Update()
	{
		static float time = 0;
		time += Time::deltaTime;

		if (player[myid])
		{
			XMFLOAT3 position = (player[myid]->transform->position / 2).xmf3;
			XMFLOAT3 lookAt = position;
			position.z = -10;
			XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
			Camera::main->GenerateViewMatrix(position, lookAt, up);
		}

		if (gameState == START)
			angle += speedRotating * Time::deltaTime;

		if (Input::GetMouseButtonDown(0) && !ready) 
		{
			ready = true;
			Message message;
			message.msgId = MESSAGE_READY;
			message.lParam = myid;
			//int retval = Sender->SendMsg(message);
			int retval = send(*sock, (char*)& message, sizeof(Message), 0);
		}
		else if (Input::GetMouseButtonDown(0) && ready) {
			Vector3 direction = NS_Vector3::Normalize((Camera::main->ScreenToWorldPoint(Input::mousePosition) - player[myid]->transform->position).xmf3);
			direction.z = 0;

			Message message;
			message.msgId = MESSAGE_REQUEST_BULLET_CREATION;
			message.mParam = myid;
			message.rParam = DirtoAngle(direction);
			//int retval = Sender->SendMsg(message);
			int retval = send(*sock, (char*)& message, sizeof(Message), 0);
		}
	}
};