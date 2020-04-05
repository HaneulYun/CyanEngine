#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>

#define SERVER_PORT 3500

struct Pawn
{
	char id; char x; char y;
};

struct SOCKETINFO
{
	WSAOVERLAPPED recv;
	WSAOVERLAPPED send;
	WSABUF recvbuf;
	WSABUF sendbuf;
	Pawn recvdata;
	Pawn senddata;

	SOCKET socket;
	SOCKADDR_IN clientAddr;
	Pawn userdata;
};

std::map<SOCKET, SOCKETINFO> clients;

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);
void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);

void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	SOCKET client_s = reinterpret_cast<int>(overlapped->hEvent);

	if (dataBytes == 0)
	{
		printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clients[client_s].clientAddr.sin_addr), ntohs(clients[client_s].clientAddr.sin_port));
		closesocket(clients[client_s].socket);
		clients.erase(client_s);
		return;
	}  // 클라이언트가 closesocket을 했을 경우

	printf("[TCP/%s:%d] %d %d\n", inet_ntoa(clients[client_s].clientAddr.sin_addr), ntohs(clients[client_s].clientAddr.sin_port), (int)clients[client_s].recvdata.x, (int)clients[client_s].recvdata.y);

	Pawn& buf = clients[client_s].recvdata;
	Pawn& pawn = clients[client_s].userdata;

	if (pawn.x == 0 && buf.x == -1 ||
		pawn.x == 7 && buf.x == 8 ||
		pawn.y == 0 && buf.y == -1 ||
		pawn.y == 7 && buf.y == 8)
		buf = pawn;
	else if (abs(pawn.x - buf.x) + abs(pawn.y - buf.y) < 2)
		pawn = buf;
	else
		buf = pawn;

	for (auto& client : clients)
	{
		client.second.senddata = pawn;
		client.second.sendbuf.buf = (char*)&client.second.senddata;
		client.second.send = {};
		client.second.send.hEvent = (HANDLE)client.second.socket;
		WSASend(client.second.socket, &(client.second.sendbuf), 1, NULL, 0, &(client.second.send), send_callback);
	}

	DWORD flags = 0;
	clients[client_s].recv = {};
	clients[client_s].recv.hEvent = (HANDLE)client_s;
	WSARecv(client_s, &clients[client_s].recvbuf, 1, 0, &flags, &(clients[client_s].recv), recv_callback);
}

void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	DWORD receiveBytes = 0;

	SOCKET client_s = reinterpret_cast<int>(overlapped->hEvent);

	if (dataBytes == 0) {
		closesocket(clients[client_s].socket);
		clients.erase(client_s);
		return;
	}  // 클라이언트가 closesocket을 했을 경우

	// WSASend(응답에 대한)의 콜백일 경우
}

int main()
{
	int retval;

	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
		return 1;

	// socket()
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET)
		err_quit("socket()");

	// bind()
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	retval = ::bind(listenSocket, (sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");

	// listen()
	retval = listen(listenSocket, 5);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN clientAddr{};
	int addrLen = sizeof(SOCKADDR_IN);

	char idCnt{ 0 };
	while (1) {
		// accept()s
		SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		clients[clientSocket] = SOCKETINFO{};
		clients[clientSocket].socket = clientSocket;
		clients[clientSocket].clientAddr = clientAddr;
		clients[clientSocket].recv = {};
		clients[clientSocket].recv.hEvent = (HANDLE)clients[clientSocket].socket;
		clients[clientSocket].recvbuf.buf = (char*)&clients[clientSocket].recvdata;
		clients[clientSocket].recvbuf.len = sizeof(Pawn);
		clients[clientSocket].send = {};
		clients[clientSocket].send.hEvent = (HANDLE)clients[clientSocket].socket;
		clients[clientSocket].sendbuf.buf = (char*)&clients[clientSocket].senddata;
		clients[clientSocket].sendbuf.len = sizeof(Pawn);
		clients[clientSocket].userdata = { idCnt++, 0, 0 };
		DWORD flags = 0;

		clients[clientSocket].senddata = clients[clientSocket].userdata;
		WSASend(clients[clientSocket].socket, &clients[clientSocket].sendbuf, 1, NULL, 0, &clients[clientSocket].send, send_callback);
		for (auto& client : clients)
		{
			for (auto& user : clients)
			{
				client.second.senddata = user.second.userdata;
				client.second.sendbuf.buf = (char*)&client.second.senddata;
				client.second.send = {};
				client.second.send.hEvent = (HANDLE)client.second.socket;
				WSASend(client.second.socket, &(client.second.sendbuf), 1, NULL, 0, &(client.second.send), send_callback);
			}
		}
		WSARecv(clients[clientSocket].socket, &clients[clientSocket].recvbuf, 1, NULL, &flags, &(clients[clientSocket].recv), recv_callback);
	}

	// closesocket()
	closesocket(listenSocket);

	// 윈속 종료
	WSACleanup();
}