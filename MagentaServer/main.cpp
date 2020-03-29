#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32.lib")
#include <stdio.h>
#include <WS2tcpip.h>

#define SERVER_PORT 3500
#define BUFSIZE 1024

int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	// socket()
	//SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	//if (listen_sock == INVALID_SOCKET) err_quit("socket()");
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);

	// bind()
	SOCKADDR_IN serverAddr;
	//ZeroMemory(&serveraddr, sizeof(serveraddr));
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	::bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
	
	// listen()
	listen(listenSocket, 5);

	SOCKADDR_IN client_addr;
	while (true) {
		int addr_size = sizeof(client_addr);
		SOCKET client_socket = accept(listenSocket, (sockaddr*)&client_addr, &addr_size);
		while (true) {
			char messageBuffer[BUFSIZE];
			int receiveBytes = recv(client_socket, messageBuffer, BUFSIZE, 0);
			if (receiveBytes > 0) printf("TRACE - Receive message : %s (%d bytes)\n", messageBuffer, receiveBytes);
			else break;
			int sendBytes = send(client_socket, messageBuffer, receiveBytes, 0);
			if (sendBytes > 0)printf("TRACE - Send message : %s (%d bytes)\n", messageBuffer, sendBytes);
		}
		closesocket(client_socket);
	}

	// closesocket()
	closesocket(listenSocket);

	// 윈속 종료
	WSACleanup();
	return 0;
}



