#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include "pch.h"

HANDLE EndEvent;
MagentaFW gMagentaFW;

int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit((char*)"socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit((char*)"bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit((char*)"listen()");

	gMagentaFW.OnCreate();
	ThreadPool::Instance();
	ThreadPool::setConnectingThread(&listen_sock);

	EndEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (EndEvent == NULL) return 1;

	DWORD eventVal;
	
	eventVal = WaitForSingleObject(EndEvent, INFINITE);
	if (eventVal != WAIT_OBJECT_0) ;	// 임시로 비워둠
	printf("done!\n");	// 임시 출력부
	SetEvent(EndEvent);

	CloseHandle(EndEvent);
	gMagentaFW.OnDestroy();
	// 윈속 종료
	WSACleanup();
	return 0;
}
