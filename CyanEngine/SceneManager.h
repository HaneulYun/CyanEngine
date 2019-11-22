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

	Vector3 AngletoDir(float angle)
	{
		Vector3 axis{ 0.0f, 0.0f, 1.0f };
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&axis.xmf3), XMConvertToRadians(angle));
		return NS_Vector3::TransformCoord(axis.xmf3, mtxRotate);
	}

	float DirtoAngle(Vector3 direction) // -180~180
	{
		Vector3 axis{ 0.0f, 1.0f, 0.0f };
		float angle = NS_Vector3::Angle(axis.xmf3, direction.xmf3);

		if (NS_Vector3::CrossProduct(axis.xmf3, direction.xmf3).z < 0.f)
			angle = -angle;

		return angle;
	}

	void Start()
	{
		scenemanager = gameObject;
		//printf("IP 입력: ");
		//char* SERVERIP = (char*)malloc(sizeof(char) * STRMAX);
		//scanf_s(" %s", SERVERIP);
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

		XMFLOAT3 position = (player[myid]->transform->position / 2).xmf3;
		XMFLOAT3 lookAt = position;
		position.z = -10;
		XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		Camera::main->GenerateViewMatrix(position, lookAt, up);

		if (gameState == START)
			angle += speedRotating * Time::deltaTime;

		if (Input::GetMouseButtonDown(0) && !ready) {
			ready = true;
			/*Message message;
			message.msgId = MESSAGE_READY;
			message.lParam = myid;
			int retval = send(*sock, (char*)& message, sizeof(Message), 0);*/
		}
		else if (Input::GetMouseButtonDown(0) && ready) {
			StarGuardian* starGuardian = player[myid]->GetComponent<StarGuardian>();

			Vector3 direction = NS_Vector3::Normalize((Camera::main->ScreenToWorldPoint(Input::mousePosition) - player[myid]->transform->position).xmf3);
			direction.z = 0;		

			starGuardian->Shoot(1, direction);

			//Send Shoot Message
			//Message message;
			//message.msgId = MESSAGE_CREATE_BULLET;
			//message.mParam = myid;
			//message.rParam = DirtoAngle(direction);
			//int retval = send(*sock, (char*)& message, sizeof(Message), 0);
		}
	}

	void CreatePlayer(int id) {
		player[id] = playerprefab->scene->Instantiate(playerprefab);
		player[id]->transform->position.xmf3 = XMFLOAT3(25.f * cos(120 * id * PI / 180.0f), 25.f * sin(120 * id * PI / 180.0f), 0.0f);
		/*player[id]->GetComponent<RevolvingBehavior>()->speedRotating = speedRotating;
		player[id]->GetComponent<RevolvingBehavior>()->angle = angle + 120 * id;*/
		XMFLOAT4 color[3] = { XMFLOAT4(1, 0, 0, 1), XMFLOAT4(0, 1, 0, 1), XMFLOAT4(0, 0, 1, 1) };
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

	void CreateBullet(int id, float angle)
	{
		Vector3 direction = AngletoDir(angle);
		player[id]->GetComponent<StarGuardian>()->Shoot(1, direction);
	}
};