#pragma once
#include <WS2tcpip.h>
#include <conio.h>
#pragma comment(lib, "Ws2_32.lib")
#include "framework.h"

#define BUFSIZE 1024


class Network : public MonoBehavior<Network>
{
private:

public:
	std::string serverIP;
	const short serverPort{ 3500 };
	SOCKET serverSocket;
	sockaddr_in serverAddr;

	bool isConnect{ false };

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
		memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(serverPort);
	}

	std::wstring ToWString(const std::string& string)
	{
		wchar_t buffer[1024];
		DWORD minSize = MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, NULL, 0);
		if (1024 < minSize)
			return L"OverFlowed";

		MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, buffer, minSize);
		return std::wstring(buffer);
	}

	void Update()
	{
		if (!isConnect)
		{
			std::wstring title(L"Input IP Here : ");
			for (char key = '0'; key <= '9'; ++key)
			{
				if (Input::GetKeyDown((KeyCode)key))
				{
					serverIP += key;
					SetWindowText(CyanApp::GetHwnd(), (title + ToWString(serverIP)).c_str());
				}
			}
			
				inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);
				SetWindowText(CyanApp::GetHwnd(), ToWString(serverIP).c_str());//title.c_str());
				int retval = connect(serverSocket, (struct sockaddr*) & serverAddr, sizeof(serverAddr));

				if (retval != SOCKET_ERROR) isConnect = true;
	
			else
			{
				serverIP += c;
			}
		}
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
