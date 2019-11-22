#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include "pch.h"

HANDLE EndEvent;
MagentaFW gMagentaFW;

int main(int argc, char* argv[])
{
	int retval;

	// ���� �ʱ�ȭ
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
	if (eventVal != WAIT_OBJECT_0) ;	// �ӽ÷� �����
	printf("done!\n");	// �ӽ� ��º�
	SetEvent(EndEvent);

	CloseHandle(EndEvent);
	gMagentaFW.OnDestroy();
	// ���� ����
	WSACleanup();
	return 0;
}
