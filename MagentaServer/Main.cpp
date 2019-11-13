#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include "ThreadPool.h"
#include "PrintErrors.h"
#include "Globals.h"

// 클라이언트와 데이터 통신
DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	while (1) {
		// 데이터 받기
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"recv()");
			break;
		}
		else if (retval == 0)
			break;

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port), buf);

		// 데이터 보내기
		retval = send(client_sock, buf, retval, 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"send()");
			break;
		}
	}

	// closesocket()
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}

extern SOCKET* listenSocket;

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

	SOCKET* listenSocket = &listen_sock;

	ThreadPool* threadPool = new ThreadPool(5, listenSocket);
	
	// 데이터 통신에 사용할 변수
	//SOCKET client_sock;
	//SOCKADDR_IN clientaddr;
	//int addrlen;
	//HANDLE hThread;
	//
	//while (1) {
	//	// accept()
	//	addrlen = sizeof(clientaddr);
	//	client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
	//	if (client_sock == INVALID_SOCKET) {
	//		err_display((char*)"accept()");
	//		break;
	//	}
	//
	//	// 접속한 클라이언트 정보 출력
	//	printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
	//		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	//
	//	// 스레드 생성
	//	hThread = CreateThread(NULL, 0, ProcessClient,
	//		(LPVOID)client_sock, 0, NULL);
	//	if (hThread == NULL) { closesocket(client_sock); }
	//	else { CloseHandle(hThread); }
	//}
	//
	//// closesocket()
	//closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}
