#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include "framework.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000

struct MOVE_PACKET
{
	char x; char y; char dx; char dy;
};

class TCPClient : public MonoBehavior<TCPClient>
{
private /*�� ������ private ������ �����ϼ���.*/:
	WSADATA wsa;
	SOCKET sock;

public  /*�� ������ public ������ �����ϼ���.*/:

private:
	friend class GameObject;
	friend class MonoBehavior<TCPClient>;
	TCPClient() = default;
	TCPClient(TCPClient&) = default;

public:
	~TCPClient()
	{
		closesocket(sock);

		WSACleanup();
	}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
		WSAStartup(MAKEWORD(2, 2), &wsa);

		sock = socket(AF_INET, SOCK_STREAM, 0);

		SOCKADDR_IN serveraddr{};
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
		serveraddr.sin_port = htons(SERVERPORT);
		connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
		MOVE_PACKET packet{};
		send(sock, (char*)&packet, sizeof(MOVE_PACKET), 0);
		recv(sock, (char*)&packet, sizeof(MOVE_PACKET), 0);
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};