#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include "framework.h"
#include "Message.h"

enum {WAIT, START, END};

class SceneManager : public MonoBehavior<SceneManager>
{
public:
	static GameObject* scenemanager;
	ObjectIDManager* objectIDmanager;
	GameObject* playerprefab{ nullptr };
	GameObject* enemyprefab[5]{ nullptr, };
	GameObject* bulletprefab[5]{ nullptr, };
	GameObject* player[3]{ nullptr, };
	GameObject* star{ nullptr };
	Thread* Sender{ nullptr };

	float speedRotating{ 30.0f };
	float angle{ 0.0f };
	float spawnRadius{ 200 };

	bool ready{ false };
	int gameState{ WAIT };
	int nplayer{ 0 };

	int myid{ 0 };
	int bulletType{ 0 };
	float elapsedTime{ 0.0f };

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
		for (int i = 0; i < 3; ++i) {
			if (player[i] != nullptr)
			{
				RevolvingBehavior* revolvingBehavior = player[i]->AddComponent<RevolvingBehavior>();
				revolvingBehavior->target = star;
				revolvingBehavior->radius = 25.0f;
				revolvingBehavior->angle = 120.0f * i;
			}
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
		player[id]->transform->position.xmf3 = XMFLOAT3(25.0f * cos(120 * id * PI / 180.0f), 25.0f * sin(120 * id * PI / 180.0f), 0.0f);
		player[id]->GetComponent<Renderer>()->material->albedo = color[id];
		for (int i = 0; i < 5; ++i)
		{
			GameObject* bullet = gameObject->scene->DuplicatePrefab(bulletprefab[i]);
			bullet->GetComponent<Renderer>()->material->albedo = color[id];
			player[id]->GetComponent<StarGuardian>()->bullet[i] = bullet;
		}
	}

	void DeletePlayer(int id)
	{
		--nplayer;
		for (int i = 0; i < 5; ++i)
		{
			Destroy(player[id]->GetComponent<StarGuardian>()->bullet[i]);
		}
		Destroy(player[id]);
		player[id] = nullptr;
	}

	void CreateBullet(int type, int id, int playerid, float angle)
	{
		Vector3 direction = AngletoDir(angle);
		if (player[playerid] != nullptr)
		{
			GameObject* bullet = player[playerid]->GetComponent<StarGuardian>()->Shoot(type, direction);
			objectIDmanager->CreateObjectID(id, bullet);
		}
		// Bullet 만들 때 ID 부여하기
	}

	void CreateEnemy(int type, int id, float radian)
	{
		GameObject* object = Instantiate(enemyprefab[type]);
		{
			object->GetComponent<Transform>()->position = Vector3(cos(radian) * spawnRadius, sin(radian) * spawnRadius, 0);
			objectIDmanager->CreateObjectID(id, object);
		}
		// Enemy 만들 때 ID 부여하기
	}
	
	void DeleteObject(int id)
	{
		objectIDmanager->DeleteObjectID(id);
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
					StarGuardian* starguardian = player[myid]->GetComponent<StarGuardian>();
					Damager* damager = starguardian->bullet[i]->AddComponent<Damager>();
					damager->isTeam = true;
					switch (i)
					{
					case 0:
						damager->SetDamageAmount(2);
						starguardian->bullet[i]->AddComponent<BoxCollider>()->extents = Vector3{ 1.5f,1.5f,1.5f };
						break;
					case 1:
						damager->SetDamageAmount(5);
						starguardian->bullet[i]->AddComponent<SphereCollider>()->radius = 4.0f;
						break;
					case 2:
						damager->SetDamageAmount(1);
						starguardian->bullet[i]->AddComponent<BoxCollider>()->extents = Vector3{ 1.5f,1.5f,1.5f };
						break;
					case 3:
						damager->SetDamageAmount(1);
						starguardian->bullet[i]->AddComponent<BoxCollider>()->extents = Vector3{ 1.0f,180.0f,1.5f };
						starguardian->bullet[i]->GetComponent<BoxCollider>()->obb = true;
						break;
					case 4:
						damager->SetDamageAmount(1);
						starguardian->bullet[i]->AddComponent<BoxCollider>()->extents = Vector3{ 1.5f,1.5f,1.5f };
						break;
					}
				}
				break;
				// 플레이어 목록의 갱신. (타 플레이어의 접속/접속해제)
			case MESSAGE_CONNECTED_IDS:
				if (curMsg.lParam && player[0] == nullptr) CreatePlayer(0);
				if (curMsg.mParam && player[1] == nullptr) CreatePlayer(1);
				if (curMsg.rParam && player[2] == nullptr) CreatePlayer(2);
				if (!curMsg.lParam && player[0] != nullptr) DeletePlayer(0);
				if (!curMsg.mParam && player[1] != nullptr) DeletePlayer(1);
				if (!curMsg.rParam && player[2] != nullptr) DeletePlayer(2);
				break;
			case MESSAGE_GAME_START:
				StartGame();
				break;
			case MESSAGE_GAME_END:
				//EndGame();
				break;
			// Bullet
			case MESSAGE_CREATE_BULLET_STRAIGHT:
			case MESSAGE_CREATE_BULLET_CANNON:
			case MESSAGE_CREATE_BULLET_SHARP:
			case MESSAGE_CREATE_BULLET_LASER:
			case MESSAGE_CREATE_BULLET_GUIDED:
				CreateBullet(curMsg.msgId - MESSAGE_CREATE_BULLET_STRAIGHT, curMsg.lParam, curMsg.mParam, curMsg.rParam);
				break;
			// Enemy Creation
			case MESSAGE_CREATE_ENEMY_COMINGRECT:
			case MESSAGE_CREATE_ENEMY_ROTATINGRECT: 
			case MESSAGE_CREATE_ENEMY_COMINGBIGRECT: 
			case MESSAGE_CREATE_ENEMY_COMINGTWOTRIANGLE:
			case MESSAGE_CREATE_ENEMY_WHIRLINGCOMINGRECT:
				CreateEnemy(curMsg.msgId - MESSAGE_CREATE_ENEMY_COMINGRECT, curMsg.lParam, curMsg.rParam);
				break;
			// Collision
			case MESSAGE_NOTIFY_COLLISION_STAR_AND_ENEMY:
				ChangeStarHealth(curMsg.mParam);
				DeleteObject(curMsg.lParam);
				break;
			case MESSAGE_DELETE_OBJECT:
				DeleteObject(curMsg.lParam);
				break;
			}
		}
	}

	void UpdateSendQueue()
	{
		while (!sendQueue.empty())
		{
			Message curMsg = sendQueue.front();
			sendQueue.pop();

			switch (curMsg.msgId)
			{
			case MESSAGE_NOTIFY_COLLISION_BULLET_AND_ENEMY:
				//curMsg.mParam = myid;
				break;
			}
			int retval = Sender->SendMsg(curMsg);
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
		if (Sender) UpdateSendQueue();

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
			sendQueue.push(message);
		}
		else if (Input::GetMouseButton(0) && gameState == START) {
			float timeCycle = bulletprefab[bulletType]->GetComponent<Bullet>()->timeCycle;
			if (timeCycle <= elapsedTime)
			{
				elapsedTime = 0.0f;
				Vector3 direction = NS_Vector3::Normalize((Camera::main->ScreenToWorldPoint(Input::mousePosition) - player[myid]->transform->position).xmf3);
				direction.z = 0;

				Message message;
				message.msgId = MESSAGE_REQUEST_BULLET_CREATION_STRAIGHT + bulletType;
				message.mParam = myid;
				message.rParam = DirtoAngle(direction);
				sendQueue.push(message);
			}
		}
	}
};