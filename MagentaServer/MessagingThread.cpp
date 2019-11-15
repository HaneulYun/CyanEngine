#include <stdio.h>
#include <queue>
#include "ThreadPool.h"
#include "MessagingThread.h"
#include "Globals.h"
#include "PrintErrors.h"

vector<queue<Message>*> ThreadPool::sendQueues;

MessagingThread::MessagingThread(int tId, LPVOID fParam)
	: Thread(tId, Messenger, fParam)
{
	ThreadPool::sendQueues.push_back(&sendQueue);
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
	Message buf;	// 임시 버퍼

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	while (1) {
		// 데이터 받기
		retval = recv(client_sock, (char*)&buf, sizeof(Message), 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"recv()");
			break;
		}
		else if (retval == 0)
			break;

		// 받은 데이터 출력
		//((char*)buf)[retval] = '\0';
		printf("[TCP/%s:%d] %c, %d, %d, %d\n", inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port), buf.msgId, buf.lParam, buf.mParam, buf.rParam);

		// 데이터 보내기
		retval = send(client_sock, (char*)&buf, sizeof(Message), 0);	
		if (retval == SOCKET_ERROR) {
			err_display((char*)"send()");
			break;
		}

		printf("루프 돌았다!\n");	// 임시 출력부
	}

	// closesocket()
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	// 종료된 클라이언트의 메시지 큐와 스레드 제거하는 메시지 넣자...

	return 0;
}

