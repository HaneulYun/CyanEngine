#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#include <iostream>
#include <map>
using namespace std;
#include <WS2tcpip.h> // winsock은 최신 api가 좀 빠져있음
#pragma comment(lib, "Ws2_32.lib")

//#define MAX_BUFFER        1024
#define SERVER_PORT        9000

struct PACKET
{
	char index;
	char x;
	char y;
};

struct ChessPiece
{
	char x;
	char y;
};

struct SOCKETINFO
{
	WSAOVERLAPPED overlapped_recv;
	WSAOVERLAPPED overlapped_send;
	WSABUF dataBuffer;
	SOCKET socket;
	char messageBuffer[sizeof(PACKET)];
	int index;
};
map <SOCKET, SOCKETINFO> clients;
ChessPiece chessPieces[10];

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);
void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	SOCKET client_s = reinterpret_cast<int>(overlapped->hEvent);
	DWORD flags = 0;
	if (dataBytes == 0)
	{
		closesocket(clients[client_s].socket);
		clients.erase(client_s);
		chessPieces[clients[client_s].index].x = -1;
		return;
	}  // 클라이언트가 closesocket을 했을 경우

	//cout << "From client : " << clients[client_s].messageBuffer << " (" << dataBytes << ") bytes)\n";
	// 데이터 처리
	PACKET packet;
	memcpy(&packet, clients[client_s].messageBuffer, sizeof(PACKET));
	//cout << "From client : " << static_cast<int>(packet.index) << " " << static_cast<int>(packet.x) << " " << static_cast<int>(packet.y) << endl;

	int index = clients[client_s].index;

	int tx = chessPieces[index].x + packet.x;
	int ty = chessPieces[index].y + packet.y;

	if (0 <= tx && tx < 8 &&
		0 <= ty && ty < 8)
	{
		chessPieces[index].x = tx;
		chessPieces[index].y = ty;
	}
	packet.index = index;
	packet.x = chessPieces[index].x;
	packet.y = chessPieces[index].y;

	// 처리된 데이터 브로드캐스트
	for (auto& d : clients)
	{
		memcpy(d.second.messageBuffer, &packet, sizeof(PACKET));
		memset(&(d.second.overlapped_send), 0, sizeof(WSAOVERLAPPED));
		d.second.overlapped_send.hEvent = (HANDLE)d.second.socket;
		WSASend(d.second.socket, &(d.second.dataBuffer), 1, NULL, 0, &(d.second.overlapped_send), send_callback);
	}

	memset(&(clients[client_s].overlapped_recv), 0, sizeof(WSAOVERLAPPED));
	clients[client_s].overlapped_recv.hEvent = (HANDLE)client_s;
	WSARecv(client_s, &clients[client_s].dataBuffer, 1, NULL, &flags, &(clients[client_s].overlapped_recv), recv_callback);
}

void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	SOCKET client_s = reinterpret_cast<int>(overlapped->hEvent);

	if (dataBytes == 0) {
		closesocket(clients[client_s].socket);
		clients.erase(client_s);
		chessPieces[clients[client_s].index].x = -1;
		return;
	}  // 클라이언트가 closesocket을 했을 경우

	// WSASend(응답에 대한)의 콜백일 경우
	//cout << "Send message : " << clients[client_s].messageBuffer << " (" << dataBytes << ") bytes)\n";
	PACKET packet;
	memcpy(&packet, clients[client_s].messageBuffer, sizeof(PACKET));
	cout << "Send message : " << static_cast<int>(packet.index) << " " << static_cast<int>(packet.x) << " " << static_cast<int>(packet.y) << endl;
}


int main(int argc, char* argv[])
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	::bind(listenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN));
	listen(listenSocket, 10);
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	memset(&clientAddr, 0, addrLen);

	memset(chessPieces, -1, 20);

	while (true) {
		SOCKET clientSocket = accept(listenSocket, (struct sockaddr*) & clientAddr, &addrLen);
		clients[clientSocket] = SOCKETINFO{};
		clients[clientSocket].socket = clientSocket;
		clients[clientSocket].dataBuffer.len = sizeof(PACKET);
		clients[clientSocket].dataBuffer.buf = clients[clientSocket].messageBuffer;
		memset(&clients[clientSocket].overlapped_recv, 0, sizeof(WSAOVERLAPPED));
		clients[clientSocket].overlapped_recv.hEvent = (HANDLE)clients[clientSocket].socket;
		memset(&clients[clientSocket].overlapped_send, 0, sizeof(WSAOVERLAPPED));
		clients[clientSocket].overlapped_send.hEvent = (HANDLE)clients[clientSocket].socket;
		DWORD flags = 0;
		
		PACKET packet;

		// 현재 보드판 상태를 새로 접속한 client에게 전송
		for (int i = 0; i < 10; ++i)
		{
			if (chessPieces[i].x != -1)
			{
				packet.index = i;
				packet.x = chessPieces[i].x;
				packet.y = chessPieces[i].y;
				memcpy(clients[clientSocket].messageBuffer, &packet, sizeof(PACKET));
				memset(&(clients[clientSocket].overlapped_send), 0, sizeof(WSAOVERLAPPED));
				clients[clientSocket].overlapped_send.hEvent = (HANDLE)clients[clientSocket].socket;
				WSASend(clientSocket, &(clients[clientSocket].dataBuffer), 1, NULL, 0, &(clients[clientSocket].overlapped_send), send_callback);
			}

		}

		int index = 0;
		for (int i = 0; i < 10; ++i)
		{
			if (chessPieces[i].x == -1)
			{
				index = i;
				chessPieces[i].x = 0;
				chessPieces[i].y = 0;
				break;
			}
		}

		packet.index = index;
		packet.x = 0;
		packet.y = 0;
		clients[clientSocket].index = index;

		// 접속하면 모든 클라이언트에게 Send
		for (auto& d : clients)
		{		
			memcpy(d.second.messageBuffer, &packet, sizeof(PACKET));
			memset(&(d.second.overlapped_send), 0, sizeof(WSAOVERLAPPED));
			d.second.overlapped_send.hEvent = (HANDLE)d.second.socket;
			WSASend(d.second.socket, &(d.second.dataBuffer), 1, NULL, 0, &(d.second.overlapped_send), send_callback);
		}

		WSARecv(clients[clientSocket].socket, &clients[clientSocket].dataBuffer, 1, NULL,
			&flags, &(clients[clientSocket].overlapped_recv), recv_callback);
	}
	closesocket(listenSocket);
	WSACleanup();
}
