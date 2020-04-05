#pragma once
#pragma comment(lib, "ws2_32")
#include <WS2tcpip.h>
#include "framework.h"

#define SERVER_IP	"127.0.0.1"
#define SERVER_PORT	3500

struct MOVE_PACKET
{
	char id; char x; char y;
};

class TCPClient : public MonoBehavior<TCPClient>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	bool isIP{ false };
	std::wstring ip;

	WSADATA WSAData;
	SOCKET serverSocket;

public  /*이 영역에 public 변수를 선언하세요.*/:
	std::vector<Pawn*> pawns;

private:
	friend class GameObject;
	friend class MonoBehavior<TCPClient>;
	TCPClient() = default;
	TCPClient(TCPClient&) = default;

public:
	~TCPClient()
	{
		closesocket(serverSocket);
		WSACleanup();
	}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
		std::wstring title(L"Yellow Project _ Input IP Here : ");

		SetWindowText(CyanApp::GetHwnd(), title.c_str());
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		if (!isIP)
		{
			for (char keyCode = '0'; keyCode <= '9'; ++keyCode)
			{
				if (Input::GetKeyDown((KeyCode)keyCode))
				{
					ip += keyCode;
					std::wstring title(L"Yellow Project _ Input IP Here : ");
					SetWindowText(CyanApp::GetHwnd(), (title+ip).c_str());
				}
			}
			if (Input::GetKeyDown(KeyCode::Period))
			{
				ip += '.';
				std::wstring title(L"Yellow Project _ Input IP Here : ");
				SetWindowText(CyanApp::GetHwnd(), (title + ip).c_str());
			}
			if (Input::GetKeyDown(KeyCode::Return))
			{
				isIP = true;

				WSAStartup(MAKEWORD(2, 0), &WSAData);
				serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);

				std::string addr;
				addr.assign(ip.begin(), ip.end());

				SOCKADDR_IN serverAddr{};
				serverAddr.sin_family = AF_INET;
				serverAddr.sin_port = htons(SERVER_PORT);
				inet_pton(AF_INET, addr.c_str(), &serverAddr.sin_addr);
				connect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
			}
		}
		else
		{
			if (Input::GetMouseButtonDown(0))
			{
				Vector3 pos = Camera::main->ScreenToWorldPoint(Input::mousePosition);
				int xIndexOnBoard = GetIndexFromPosition(pos.x);
				int yIndexOnBoard = GetIndexFromPosition(pos.y);

				MOVE_PACKET packet{ 0, xIndexOnBoard, yIndexOnBoard };
				send(serverSocket, (char*)&packet, sizeof(MOVE_PACKET), 0);
				recv(serverSocket, (char*)&packet, sizeof(MOVE_PACKET), 0);

				pawns[0]->SetPositionByIndex(packet.x, packet.y);
			}
		}
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
	int GetIndexFromPosition(float d)
	{
		return floorf((d + 300) / 75);
	}
};