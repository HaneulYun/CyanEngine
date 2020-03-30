#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000

struct PACKET
{
	char x; 
	char y;
};

struct ChessPiece
{
	char x;
	char y;
};


int main(int argc, char* argv[])
{
	ChessPiece pawn{ 0,0 };
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));


	// listen()
	retval = listen(listen_sock, SOMAXCONN);

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	PACKET packet{};

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

		// 접속한 클라이언트 정보 출력
		char ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
		WSAAddressToString((SOCKADDR*)&clientaddr, sizeof(clientaddr),
			NULL, ipaddr, &ipaddrlen);
		printf("\n[TCP 서버] 클라이언트 접속: %s\n", ipaddr);

		// 클라이언트와 데이터 통신
		while (1) {
			// 데이터 받기
			retval = recv(client_sock, (char*)&packet, sizeof(PACKET), 0);
			if (retval == 0)
				break;

			// 받은 데이터 출력
			printf("[TCP/%s] %d, %d\n", ipaddr, packet.x, packet.y);

			// 데이터 처리
			int tx = pawn.x + packet.x;
			int ty = pawn.y + packet.y;

			if (0 <= tx && tx < 8 &&
				0 <= ty && ty < 8)
			{
				pawn.x = tx;
				pawn.y = ty;
			}
			packet.x = pawn.x;
			packet.y = pawn.y;

			// 데이터 보내기
			retval = send(client_sock, (char*)&packet, retval, 0);
			
		}

		// closesocket()
		closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료: %s\n", ipaddr);
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}
