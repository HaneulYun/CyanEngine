#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include "framework.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000

struct MOVE_PACKET
{
	char x; char y;
};

class TCPClient : public MonoBehavior<TCPClient>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	bool isIP{ false };
	std::wstring ip;

	WSADATA wsa;
	SOCKET sock;

public  /*이 영역에 public 변수를 선언하세요.*/:
	Pawn* pawn;

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

				WSAStartup(MAKEWORD(2, 2), &wsa);

				sock = socket(AF_INET, SOCK_STREAM, 0);

				std::string addr;
				addr.assign(ip.begin(), ip.end());

				SOCKADDR_IN serveraddr{};
				serveraddr.sin_family = AF_INET;
				serveraddr.sin_addr.s_addr = inet_addr(addr.c_str());
				serveraddr.sin_port = htons(SERVERPORT);
				connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
			}
		}
		else
		{
			if (Input::GetMouseButtonDown(0))
			{
				Vector3 pos = Camera::main->ScreenToWorldPoint(Input::mousePosition);
				int xIndexOnBoard = GetIndexFromPosition(pos.x);
				int yIndexOnBoard = GetIndexFromPosition(pos.y);

				MOVE_PACKET packet{ xIndexOnBoard, yIndexOnBoard };
				send(sock, (char*)&packet, sizeof(MOVE_PACKET), 0);
				recv(sock, (char*)&packet, sizeof(MOVE_PACKET), 0);

				pawn->SetPositionByIndex(packet.x, packet.y);
			}
		}
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
	int GetIndexFromPosition(float d)
	{
		return (d + 300) / 75;
	}
};