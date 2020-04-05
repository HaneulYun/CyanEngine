#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include "framework.h"
#include "chessPiece.h"

#define SERVERPORT 9000

struct PACKET
{
	char index;
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
	chessPiece* pawns[10];

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
		// 윈속 초기화
		WSAStartup(MAKEWORD(2, 0), &wsa);

		// socket()
		sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
		unsigned long noblock = 1; 
		int nRet = ioctlsocket(sock, FIONBIO, &noblock);

		std::wstring title(L"IP를 입력해주세요 : ");
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
					std::wstring title(L"IP를 입력해주세요 : ");
					SetWindowText(CyanApp::GetHwnd(), (title + ip).c_str());
				}
			}
			if (Input::GetKeyDown(KeyCode::Period))
			{
				ip += '.';
				std::wstring title(L"IP를 입력해주세요 : ");
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
			PACKET packet;
			if(recv(sock, (char*)&packet, sizeof(PACKET), 0) > 0)
				pawns[packet.index]->move(packet.x, packet.y);

			if (Input::GetKeyDown(KeyCode::W))
			{
				packet.index = 0;
				packet.x = 0;
				packet.y = -1;
				send(sock, (char*)&packet, sizeof(PACKET), 0);
				
			}
			if (Input::GetKeyDown(KeyCode::A))
			{
				packet.index = 0;
				packet.x = -1;
				packet.y = 0;
				send(sock, (char*)&packet, sizeof(PACKET), 0);
			}
			if (Input::GetKeyDown(KeyCode::S))
			{
				//Move(0, +1);
				packet.index = 0;
				packet.x = 0;
				packet.y = 1;
				send(sock, (char*)&packet, sizeof(PACKET), 0);
			}
			if (Input::GetKeyDown(KeyCode::D))
			{
				//Move(+1, 0);
				packet.index = 0;
				packet.x = 1;
				packet.y = 0;
				send(sock, (char*)&packet, sizeof(PACKET), 0);
			}
		}


	}
};
