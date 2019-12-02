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
	TextureShader* shader;

public:
	static GameObject* scenemanager;
	GameObject* playerprefab{ nullptr };
	GameObject* enemyprefab[5]{ nullptr, };
	GameObject* bulletprefab[5]{ nullptr, };
	GameObject* player[3]{ nullptr, };
	GameObject* star{ nullptr };
	Thread* Sender{ nullptr };

	float speedRotating{ 30.f };
	float angle{ 0.0f };
	float spawnRadius{ 200 };

	bool ready{ false };
	int gameState{ WAIT };
	int nplayer{ 0 };

	int myid{ 0 };
	int bulletType{ 1 };
	float elapsedTime{ 0.f };


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

	void EndGame()
	{
		gameState = END;
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

	void ChangeStarHealth(int health)
	{
		star->GetComponent<Star>()->health = health;
	}

	void UpdateRecvQueue()
	{
		while (!recvQueue.empty())
		{
			EnterCriticalSection(&rqcs);
			Message curMsg = recvQueue.front();
			recvQueue.pop();
			LeaveCriticalSection(&rqcs);
			/////////////////////////////////////////

			switch (curMsg.msgId)
			{
				// 내가 접속했을 때
			case MESSAGE_YOUR_ID:
				myid = curMsg.lParam;
				angle = curMsg.mParam;
				CreatePlayer(curMsg.lParam);
				for (int i = 0; i < 5; ++i)
				{
					player[myid]->GetComponent<StarGuardian>()->bullet[i]->AddComponent<Damager>();
				}
				break;
				// 플레이어 목록의 갱신. (타 플레이어의 접속/접속해제)
			case MESSAGE_CONNECTED_IDS:
				if (curMsg.lParam && player[0] == nullptr)
					CreatePlayer(0);
				if (curMsg.mParam && player[1] == nullptr)
					CreatePlayer(1);
				if (curMsg.rParam && player[2] == nullptr)
					CreatePlayer(2);
				break;
			case MESSAGE_GAME_START:
				StartGame();
				break;
			case MESSAGE_GAME_END:
				EndGame();
				break;
			// Bullet
			case MESSAGE_CREATE_BULLET_STRAIGHT:
			case MESSAGE_CREATE_BULLET_CANNON:
			case MESSAGE_CREATE_BULLET_SHARP:
			case MESSAGE_CREATE_BULLET_LASER:
			case MESSAGE_CREATE_BULLET_GUIDED:
				CreateBullet(curMsg.msgId - MESSAGE_CREATE_BULLET_STRAIGHT, curMsg.mParam, curMsg.rParam);
				break;
			// Enemy Creation
			case MESSAGE_CREATE_ENEMY_COMINGRECT:
				CreateEnemy(0, curMsg.rParam);
				break;
			// Collision
			case MESSAGE_NOTIFY_COLLISION_STAR_AND_ENEMY:
				ChangeStarHealth(curMsg.mParam);
				break;
			}
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
		elapsedTime += Time::deltaTime;

		UpdateRecvQueue();

		if (player[myid])
		{
			XMFLOAT3 position = (player[myid]->transform->position / 2).xmf3;
			XMFLOAT3 lookAt = position;
			position.z = -10;
			XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
			Camera::main->GenerateViewMatrix(position, lookAt, up);
		}

		if (gameState == START)
		{
			angle += speedRotating * Time::deltaTime;
		}

		if (Input::GetMouseButtonDown(0) && !ready) 
		{
			ready = true;
			Message message;
			message.msgId = MESSAGE_READY;
			message.lParam = myid;
			if (Sender)
				int retval = Sender->SendMsg(message);
		}
		else if (Input::GetMouseButtonDown(0) && gameState == START) {
			float timeCycle = bulletprefab[bulletType]->GetComponent<Bullet>()->timeCycle;
			if (timeCycle <= elapsedTime)
			{
				elapsedTime = 0.f;
				Vector3 direction = NS_Vector3::Normalize((Camera::main->ScreenToWorldPoint(Input::mousePosition) - player[myid]->transform->position).xmf3);
				direction.z = 0;

				Message message;
				message.msgId = MESSAGE_REQUEST_BULLET_CREATION_STRAIGHT + bulletType;
				message.mParam = myid;
				message.rParam = DirtoAngle(direction);
				if (Sender)
					int retval = Sender->SendMsg(message);
			}
		}
	}
};