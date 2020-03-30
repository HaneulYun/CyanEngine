#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
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

	// ���� �ʱ�ȭ
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

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	PACKET packet{};

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

		// ������ Ŭ���̾�Ʈ ���� ���
		char ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
		WSAAddressToString((SOCKADDR*)&clientaddr, sizeof(clientaddr),
			NULL, ipaddr, &ipaddrlen);
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: %s\n", ipaddr);

		// Ŭ���̾�Ʈ�� ������ ���
		while (1) {
			// ������ �ޱ�
			retval = recv(client_sock, (char*)&packet, sizeof(PACKET), 0);
			if (retval == 0)
				break;

			// ���� ������ ���
			printf("[TCP/%s] %d, %d\n", ipaddr, packet.x, packet.y);

			// ������ ó��
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

			// ������ ������
			retval = send(client_sock, (char*)&packet, retval, 0);
			
		}

		// closesocket()
		closesocket(client_sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ����: %s\n", ipaddr);
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}
