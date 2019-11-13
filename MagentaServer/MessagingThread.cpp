#include <stdio.h>
#include "MessagingThread.h"
#include "Globals.h"
#include "PrintErrors.h"

MessagingThread::MessagingThread(int tId, LPVOID fParam)
	: Thread(tId, Messenger, fParam)
{

}

MessagingThread::~MessagingThread()
{

}

DWORD WINAPI Messenger(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	
	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);
	
	while (1) {
		// 데이터 받기
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"recv()");
			break;
		}
		else if (retval == 0)
			break;
	
		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port), buf);
	}
	
	// closesocket()
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}

