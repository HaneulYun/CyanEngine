#pragma once
#pragma comment(lib, "ws2_32")
#include <WS2tcpip.h>
#include "framework.h"

#define SERVER_IP	"127.0.0.1"
#define SERVER_PORT	3500

struct MOVE_PACKET
{
	char x; char y;
};

class TCPClient : public MonoBehavior<TCPClient>
{
private /*�� ������ private ������ �����ϼ���.*/:
	bool isIP{ false };
	std::wstring ip;

	WSADATA WSAData;
	SOCKET serverSocket;

public  /*�� ������ public ������ �����ϼ���.*/:
	Pawn* pawn;

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

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
		std::wstring title(L"Yellow Project _ Input IP Here : ");

		SetWindowText(CyanApp::GetHwnd(), title.c_str());
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
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

				MOVE_PACKET packet{ xIndexOnBoard, yIndexOnBoard };
				send(serverSocket, (char*)&packet, sizeof(MOVE_PACKET), 0);
				recv(serverSocket, (char*)&packet, sizeof(MOVE_PACKET), 0);

				pawn->SetPositionByIndex(packet.x, packet.y);
			}
		}
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
	int GetIndexFromPosition(float d)
	{
		return floorf((d + 300) / 75);
	}
};