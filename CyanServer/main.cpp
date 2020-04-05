#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVER_PORT 3500

struct Pawn
{
	char x; char y;
};

struct MOVE_PACKET
{
	char x; char y;
};

void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
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

int main()
{
	int retval;

	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
		return 1;

	// socket()
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET)
		err_quit("socket()");

	// bind()
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	retval = ::bind(listenSocket, (sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");

	// listen()
	retval = listen(listenSocket, 5);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientAddr{};
	int addrlen;
	MOVE_PACKET buf;

	while (1) {
		// accept()
		addrlen = sizeof(clientAddr);
		client_sock = accept(listenSocket, (SOCKADDR*)&clientAddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		Pawn pawn{ 0, 0 };
		// 클라이언트와 데이터 통신
		while (1) {
			// 데이터 받기
			retval = recv(client_sock, (char*)&buf, sizeof(MOVE_PACKET), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			// 받은 데이터 출력
			printf("[TCP/%s:%d] %d %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), (int)buf.x, (int)buf.y);

			if (pawn.x == 0 && buf.x == -1 ||
				pawn.x == 7 && buf.x == 8 ||
				pawn.y == 0 && buf.y == -1 ||
				pawn.y == 7 && buf.y == 8)
				buf = { pawn.x, pawn.y };
			else if (abs(pawn.x - buf.x) + abs(pawn.y - buf.y) < 2)
				pawn = { buf.x, buf.y };
			else
				buf = { pawn.x, pawn.y };

			// 데이터 보내기
			retval = send(client_sock, (char*)&buf, sizeof(MOVE_PACKET), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}
		}

		// closesocket()
		closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
	}

	// closesocket()
	closesocket(listenSocket);

	// 윈속 종료
	WSACleanup();
}