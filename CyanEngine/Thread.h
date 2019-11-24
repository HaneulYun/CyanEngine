#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#pragma once
#include "framework.h"
#include "Message.h"

class Thread : public Component
{
private:
	// �� ������ private ������ �����ϼ���.
	int retval;
	WSADATA wsa;
	SOCKET sock;

public:
	// �� ������ public ������ �����ϼ���.
	//192.168.35.35
	//192.168.35.95 ��
	const char* severip{ "192.168.35.35" };
	const short severport{ 9000 };

	

private:
	friend class GameObject;
	Thread() = default;
	Thread(Thread&) = default;
	
	void err_quit(const char* msg)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL);
		MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
		LocalFree(lpMsgBuf);
		exit(1);
	}	
	void err_display(const char* msg)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL);
		printf("[%s] %s", msg, (char*)lpMsgBuf);
		LocalFree(lpMsgBuf);
	}

	static DWORD WINAPI RecvThread(LPVOID sock)
	{
		Message buf;
		int retval;
		SceneManager* sceneManager = SceneManager::scenemanager->GetComponent<SceneManager>();

		while (1) {
			retval = recv((SOCKET)sock, (char*)&buf, sizeof(Message), 0);
			if (retval == SOCKET_ERROR) {
				;//err_display("recv()");
			}
			//printf("recvbuf: %c, %d, %d, %d\n", buf.msgId, buf.lParam, buf.mParam, buf.rParam);

			int id = buf.lParam;
			EnterCriticalSection(&rqcs);
			switch (buf.msgId)
			{
			// ���� �������� ��
			case MESSAGE_YOUR_ID:
				id = buf.lParam;
				sceneManager->myid = id;
				sceneManager->angle = buf.mParam;
				sceneManager->CreatePlayer(id);
				break;
			// �÷��̾� ����� ����. (Ÿ �÷��̾��� ����/��������)
			case MESSAGE_CONNECTED_IDS:
				if (buf.lParam && sceneManager->player[0] == nullptr)
					sceneManager->CreatePlayer(0);
				if (buf.mParam && sceneManager->player[1] == nullptr)
					sceneManager->CreatePlayer(1);
				if (buf.rParam && sceneManager->player[2] == nullptr)
					sceneManager->CreatePlayer(2);
				break;
			case MESSAGE_GAME_START:
				sceneManager->StartGame();
				break;
			case MESSAGE_CREATE_BULLET:
				sceneManager->CreateBullet(buf.mParam, buf.rParam);
				break;
			case MESSAGE_CREATE_ENEMY_COMINGRECT:
				sceneManager->CreateEnemy(1, buf.rParam);
				break;
			}
			LeaveCriticalSection(&rqcs);
		}
		return 0;
	}

public:
	~Thread() {
		// closesocket()
		closesocket(sock);
		WSACleanup();

		DeleteCriticalSection(&rqcs);
	}
	virtual Component* Duplicate() { return new Thread; }
	virtual Component* Duplicate(Component* component) { return new Thread(*(Thread*)component); }

	void Start()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.		
		InitializeCriticalSection(&rqcs);

		retval = WSAStartup(MAKEWORD(2, 2), &wsa);
		if(retval != 0)
			err_quit("WSAStartup error");

		// socket()
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET) err_quit("socket()");
		//printf("IP �Է�: ");
		//char* SERVERIP = (char*)malloc(sizeof(char) * STRMAX);
		//scanf_s(" %s", SERVERIP);
		SceneManager::scenemanager->GetComponent<SceneManager>()->sock = &sock;
		// connect()
		SOCKADDR_IN serveraddr;
		ZeroMemory(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = inet_addr(severip);
		serveraddr.sin_port = htons(severport);
		retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR) err_quit("connect()");

		// ������ ����
		HANDLE hRecv = CreateThread(NULL, 0, RecvThread, (LPVOID)sock, 0, NULL);
		if (hRecv != NULL) CloseHandle(hRecv);
	}

	void Update()
	{
		// ������Ʈ �ڵ带 �ۼ��ϼ���.
	}


	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};