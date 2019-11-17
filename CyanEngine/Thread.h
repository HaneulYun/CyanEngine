#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#pragma once
#include <winsock2.h>
#include "framework.h"
#include "Message.h"

class Thread : public Component
{
private:
	// 이 영역에 private 변수를 선언하세요.
	int retval;
	WSADATA wsa;
	SOCKET sock;

public:
	// 이 영역에 public 변수를 선언하세요.
	const char* severip{ "127.0.0.1" };
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

		while (1) {
			retval = recv((SOCKET)sock, (char*)&buf, sizeof(Message), 0);
			if (retval == SOCKET_ERROR) {
				;//err_display("recv()");
			}
			//printf("recvbuf: %c, %d, %d, %d\n", buf.msgId, buf.lParam, buf.mParam, buf.rParam);

			//EnterCriticalSection(&rqcs);
			switch (buf.msgId)
			{
			case 0:
				// 플레이어 접속 메시지
				for (int i = 0; i < 3; ++i) {
					if (SceneManager::player[i] == nullptr) {
						SceneManager::player[i] = SceneManager::player[i]->Instantiate(SceneManager::player[0]);
						SceneManager::player[i]->GetComponent<RevolvingBehavior>()->speedRotating = SceneManager::scenemanager->GetComponent<SceneManager>()->speedRotating;
						SceneManager::player[i]->GetComponent<RevolvingBehavior>()->angle = SceneManager::scenemanager->GetComponent<SceneManager>()->angle + 120 * i;
						XMFLOAT4 color[3] = { XMFLOAT4(1, 0, 0, 1), XMFLOAT4(0, 1, 0, 1), XMFLOAT4(0, 0, 1, 1) };
						SceneManager::player[i]->GetComponent<Renderer>()->material->albedo = color[buf.lParam];
					}
				}
				break;
			}
			//LeaveCriticalSection(&rqcs);
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
		// 초기화 코드를 작성하세요.		
		InitializeCriticalSection(&rqcs);

		retval = WSAStartup(MAKEWORD(2, 2), &wsa);
		if(retval != 0)
			err_quit("WSAStartup error");

		// socket()
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET) err_quit("socket()");
		//printf("IP 입력: ");
		//char* SERVERIP = (char*)malloc(sizeof(char) * STRMAX);
		//scanf_s(" %s", SERVERIP);

		// connect()
		SOCKADDR_IN serveraddr;
		ZeroMemory(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = inet_addr(severip);
		serveraddr.sin_port = htons(severport);
		retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR) err_quit("connect()");

		// 스레드 생성
		HANDLE hRecv = CreateThread(NULL, 0, RecvThread, (LPVOID)sock, 0, NULL);
		if (hRecv != NULL) CloseHandle(hRecv);
	}

	void Update()
	{
		// 업데이트 코드를 작성하세요.
	}


	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
};