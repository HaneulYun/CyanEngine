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
	unsigned char index;
	unsigned char x;
	unsigned char y;
};

struct SOCKETINFO
{
	WSAOVERLAPPED overlapped_recv;
	WSAOVERLAPPED overlapped_send;
	WSABUF dataBuffer;
	char messageBuffer[sizeof(PACKET)];
};

SOCKETINFO socketInfo;
chessPiece* chessPiece[10]{ NULL };

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	SOCKET client_s = reinterpret_cast<int>(overlapped->hEvent);

	if (dataBytes == 0)
	{
		closesocket(client_s);
		return;
	}  // 클라이언트가 closesocket을 했을 경우

	//cout << "From client : " << clients[client_s].messageBuffer << " (" << dataBytes << ") bytes)\n";

	// 데이터 처리
	PACKET packet;
	memcpy(&packet, socketInfo.messageBuffer, sizeof(PACKET));

	pawn->move(packet.x, packet.y);

	memset(&(socketInfo.overlapped_recv), 0, sizeof(WSAOVERLAPPED));
	socketInfo.overlapped_recv.hEvent = (HANDLE)client_s;
	WSARecv(client_s, &socketInfo.dataBuffer, 1, NULL,
		0, &(socketInfo.overlapped_recv), recv_callback);
}

void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	SOCKET client_s = reinterpret_cast<int>(overlapped->hEvent);

	if (dataBytes == 0) {
		closesocket(client_s);
		return;
	}  // 클라이언트가 closesocket을 했을 경우

	// WSASend(응답에 대한)의 콜백일 경우
	//std::cout << "Send message : " << clients[client_s].messageBuffer << " (" << dataBytes << ") bytes)\n";
}

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
		// 윈속 초기화
		WSAStartup(MAKEWORD(2, 2), &wsa);

		// socket()
		sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

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

				socketInfo.dataBuffer.len = sizeof(SOCKET);
				socketInfo.dataBuffer.buf = socketInfo.messageBuffer;
				memset(&(socketInfo.overlapped_recv), 0, sizeof(WSAOVERLAPPED));
				socketInfo.overlapped_recv.hEvent = (HANDLE)sock;
				WSARecv(sock, &socketInfo.dataBuffer, 1, NULL,
					0, &(socketInfo.overlapped_recv), recv_callback);
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
		//recv(sock, (char*)&packet, sizeof(PACKET), 0);

		pawn->move(packet.x, packet.y);
	}


};
