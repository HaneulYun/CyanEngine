#include <iostream>
#include <map>
using namespace std;
#include <WS2tcpip.h> // winsock�� �ֽ� api�� �� ��������
#pragma comment(lib, "Ws2_32.lib")

//#define MAX_BUFFER        1024
#define SERVER_PORT        9000

struct PACKET
{
	unsigned char index;
	unsigned char x;
	unsigned char y;
};

struct ChessPiece
{
	unsigned char x;
	unsigned char y;
};

struct SOCKETINFO
{
	WSAOVERLAPPED overlapped_recv;
	WSAOVERLAPPED overlapped_send;
	WSABUF dataBuffer;
	SOCKET socket;
	char messageBuffer[sizeof(PACKET)];
};
map <SOCKET, SOCKETINFO> clients;
ChessPiece chessPieces[10]{ {-1,-1}, };

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);
void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	SOCKET client_s = reinterpret_cast<int>(overlapped->hEvent);

	if (dataBytes == 0)
	{
		closesocket(clients[client_s].socket);
		clients.erase(client_s);
		return;
	}  // Ŭ���̾�Ʈ�� closesocket�� ���� ���

	cout << "From client : " << clients[client_s].messageBuffer << " (" << dataBytes << ") bytes)\n";

	// ������ ó��
	PACKET packet;
	memcpy(&packet, clients[client_s].messageBuffer, sizeof(PACKET));

	int tx = chessPieces[packet.index].x + packet.x;
	int ty = chessPieces[packet.index].y + packet.y;

	if (0 <= tx && tx < 8 &&
		0 <= ty && ty < 8)
	{
		chessPieces[packet.index].x = tx;
		chessPieces[packet.index].y = ty;
	}
	packet.x = chessPieces[packet.index].x;
	packet.y = chessPieces[packet.index].y;

	// ó���� ������ ��ε�ĳ��Ʈ
	for (auto d : clients)
	{
		memcpy(d.second.messageBuffer, &packet, sizeof(PACKET));
		memset(&(d.second.overlapped_send), 0, sizeof(WSAOVERLAPPED));
		d.second.overlapped_send.hEvent = (HANDLE)d.second.socket;
		WSASend(d.second.socket, &(d.second.dataBuffer), 1, NULL, 0, &(d.second.overlapped_send), send_callback);
	}

	memset(&(clients[client_s].overlapped_recv), 0, sizeof(WSAOVERLAPPED));
	clients[client_s].overlapped_recv.hEvent = (HANDLE)client_s;
	WSARecv(clients[client_s].socket, &clients[client_s].dataBuffer, 1, NULL,
		0, &(clients[client_s].overlapped_recv), recv_callback);
}

void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	SOCKET client_s = reinterpret_cast<int>(overlapped->hEvent);

	if (dataBytes == 0) {
		closesocket(clients[client_s].socket);
		clients.erase(client_s);
		return;
	}  // Ŭ���̾�Ʈ�� closesocket�� ���� ���

	// WSASend(���信 ����)�� �ݹ��� ���
	cout << "Send message : " << clients[client_s].messageBuffer << " (" << dataBytes << ") bytes)\n";
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

		PACKET packet;
		packet.index = index;
		packet.x = 0;
		packet.y = 0;

		// ���� ������ ���¸� ���� ������ client���� ����
		//@@@@@

		// �����ϸ� ��� Ŭ���̾�Ʈ���� Send
		for (auto d : clients)
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


	/*

	ChessPiece pawn{ 0,0 };
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));


	// listen()
	retval = listen(listen_sock, SOMAXCONN);

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	PACKET packet{};

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

		// ������ Ŭ���̾�Ʈ ���� ���
		char ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
		WSAAddressToString((SOCKADDR*)&clientaddr, sizeof(clientaddr),
			NULL, ipaddr, &ipaddrlen);
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: %s\n", ipaddr);

		// Ŭ���̾�Ʈ�� ������ ���
		while (1) {
			// ������ �ޱ�
			retval = recv(client_sock, (char*)&packet, sizeof(PACKET), 0);
			if (retval == 0)
				break;

			// ���� ������ ���
			printf("[TCP/%s] %d, %d\n", ipaddr, packet.x, packet.y);

			// ������ ó��
			int tx = pawn.x + packet.x;
			int ty = pawn.y + packet.y;

			if (0 <= tx && tx < 8 &&
				0 <= ty && ty < 8)
			{
				pawn.x = tx;
				pawn.y = ty;
			}
			packet.x = pawn.x;
			packet.y = pawn.y;

			// ������ ������
			retval = send(client_sock, (char*)&packet, retval, 0);
			
		}

		// closesocket()
		closesocket(client_sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ����: %s\n", ipaddr);
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
	*/
}
