#pragma once
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#include "framework.h"

#define BUFSIZE 1024
#define SERVER_PORT 3500

class Network : public MonoBehavior<Network>
{
private:

public:
	const char* serverIP = "127.0.0.1";
	SOCKET serverSocket;

private:
	friend class GameObject;
	friend class MonoBehavior<Network>;
	Network() = default;
	Network(Network&) = default;

public:
	~Network() {
		closesocket(serverSocket);
		WSACleanup();
	}

	void Start()
	{
		WSADATA WSAData;
		WSAStartup(MAKEWORD(2, 2), &WSAData);

		serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
		sockaddr_in serverAddr;
		memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(SERVER_PORT);
		inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
		connect(serverSocket, (struct sockaddr*) & serverAddr, sizeof(serverAddr));
	}

	void Update()
	{

	}

	Vector3 communicate(char msgID, const Vector3& curPos)
	{
		int sendBytes = send(serverSocket, (char*)&msgID, 1, 0);
		if (sendBytes > 0)
		{
			Message msg;
			int receiveBytes = recv(serverSocket, (char*)&msg, sizeof(Message), 0);
			if (receiveBytes > 0)
			{
				Vector3 pos;
				pos.x = msg.x;
				pos.y = msg.y;
				pos.z = msg.z;

				return pos;
			}
		}
		return curPos;
	}
};
