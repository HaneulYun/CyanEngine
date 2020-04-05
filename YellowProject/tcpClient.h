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

	WSAOVERLAPPED recv;
	WSABUF recvbuf;
	MOVE_PACKET recvdata;

	bool update_server{ 0 };

	DWORD flags{ 0 };

public  /*이 영역에 public 변수를 선언하세요.*/:
	std::vector<Pawn*> pawns;
	std::vector<Pawn*> allocPawns;
	std::map<char, Pawn*> users;

	char myID{ -1 };

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
					SetWindowText(CyanApp::GetHwnd(), (title + ip).c_str());
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
				serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

				std::string addr;
				addr.assign(ip.begin(), ip.end());

				SOCKADDR_IN serverAddr{};
				serverAddr.sin_family = AF_INET;
				serverAddr.sin_port = htons(SERVER_PORT);
				inet_pton(AF_INET, addr.c_str(), &serverAddr.sin_addr);
				WSAConnect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr), 0, 0, 0, 0);

				recv = {};
				recv.hEvent = (HANDLE)this;
				recvbuf.buf = (char*)&recvdata;
				recvbuf.len = sizeof(MOVE_PACKET);
				WSARecv(serverSocket, &recvbuf, 1, NULL, &flags, &recv, recv_callback);
			}
		}
		else
		{
			if (Input::GetMouseButtonDown(0))
			{
				Vector3 pos = Camera::main->ScreenToWorldPoint(Input::mousePosition);
				int xIndexOnBoard = GetIndexFromPosition(pos.x);
				int yIndexOnBoard = GetIndexFromPosition(pos.y);

				MOVE_PACKET packet{ myID, xIndexOnBoard, yIndexOnBoard };
				send(serverSocket, (char*)&packet, sizeof(MOVE_PACKET), 0);
			}
		}
		if (update_server)
		{
			update_server = false;
			for (auto& d : users)
				d.second->SetPositionCurrentIndex();
		}
		SleepEx(10, TRUE);
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
	int GetIndexFromPosition(float d)
	{
		return floorf((d + 300) / 75);
	}

	static void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
	{
		TCPClient* tcpClient = reinterpret_cast<TCPClient*>(overlapped->hEvent);
		SOCKET client_s = tcpClient->serverSocket;

		if (dataBytes == 0)
		{
			closesocket(client_s);
			return;
		}

		MOVE_PACKET& buf = tcpClient->recvdata;
		if (tcpClient->users.find(buf.id) == tcpClient->users.end() && tcpClient->pawns.size())
		{
			tcpClient->allocPawns.push_back(tcpClient->pawns.back());
			tcpClient->pawns.pop_back();
			tcpClient->users[buf.id] = tcpClient->allocPawns.back();
		}
		tcpClient->update_server = true;
		tcpClient->users[buf.id]->SetPositionIndex(buf.x, buf.y);
		if (tcpClient->myID == -1)
			tcpClient->myID = buf.id;

		DWORD flags{ 0 };
		tcpClient->recv.hEvent = (HANDLE)tcpClient;
		tcpClient->recvbuf.buf = (char*)&tcpClient->recvdata;
		tcpClient->recvbuf.len = sizeof(MOVE_PACKET);
		WSARecv(tcpClient->serverSocket, &tcpClient->recvbuf, 1, NULL, &flags, &tcpClient->recv, recv_callback);
	}
};