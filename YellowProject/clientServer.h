#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include "framework.h"

#define SERVERPORT 9000

struct PACKET
{
	char x;
	char y;
};

class clientServer : public MonoBehavior<clientServer>
{
private:
	WSADATA wsa;
	SOCKET sock;

	bool inputIP{ false };
	std::wstring ip;

public:
	chessPiece* pawn;

private:
	friend class GameObject;
	friend class MonoBehavior<clientServer>;
	clientServer() = default;
	clientServer(clientServer&) = default;

public:
	~clientServer() 
	{
		closesocket(sock);

		WSACleanup();
	}

	void Start()
	{
		// ���� �ʱ�ȭ
		WSAStartup(MAKEWORD(2, 2), &wsa);

		// socket()
		sock = socket(AF_INET, SOCK_STREAM, 0);

		std::wstring title(L"IP�� �Է����ּ��� : ");
		SetWindowText(CyanApp::GetHwnd(), (title).c_str());
	}

	void Update()
	{
		if (!inputIP)
		{
			for (char keyCode = '0'; keyCode <= '9'; ++keyCode)
			{
				if (Input::GetKeyDown((KeyCode)keyCode))
				{
					ip += keyCode;
					std::wstring title(L"IP�� �Է����ּ��� : ");
					SetWindowText(CyanApp::GetHwnd(), (title + ip).c_str());
				}
			}
			if (Input::GetKeyDown(KeyCode::Period))
			{
				ip += '.';
				std::wstring title(L"IP�� �Է����ּ��� : ");
				SetWindowText(CyanApp::GetHwnd(), (title + ip).c_str());
			}
			if (Input::GetKeyDown(KeyCode::Return))
			{
				inputIP = true;

				std::string serverIP;
				serverIP.assign(ip.begin(), ip.end());

				SOCKADDR_IN serveraddr{};
				serveraddr.sin_family = AF_INET;
				serveraddr.sin_addr.s_addr = inet_addr(serverIP.c_str());
				serveraddr.sin_port = htons(SERVERPORT);
				connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
			}
		}
		else
		{
			if (Input::GetKeyDown(KeyCode::W))
			{
				Move(0, -1);
			}
			if (Input::GetKeyDown(KeyCode::A))
			{
				Move(-1, 0);
			}
			if (Input::GetKeyDown(KeyCode::S))
			{
				Move(0, +1);
			}
			if (Input::GetKeyDown(KeyCode::D))
			{
				Move(+1, 0);
			}
		}


	}

	void Move(int x, int y) 
	{
		PACKET packet{ x, y };
		send(sock, (char*)&packet, sizeof(PACKET), 0);
		recv(sock, (char*)&packet, sizeof(PACKET), 0);

		pawn->move(packet.x, packet.y);
	}

};
