#include <stdio.h>
#include <queue>
#include "ThreadPool.h"
#include "MessagingThread.h"
#include "Globals.h"
#include "PrintErrors.h"

queue<Message> ThreadPool::recvQueue;
CRITICAL_SECTION ThreadPool::rqcs;

MessagingThread::MessagingThread(int tId, LPVOID fParam)
	: Thread(tId, Messenger, fParam)
{
}

MessagingThread::~MessagingThread()
{
}

DWORD WINAPI Messenger(LPVOID arg)
{
	int id = ThreadPool::getNThreads() - 1;
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	Message buf;	// 임시 버퍼

	// 이벤트를 위한 변수
	DWORD rEvent;

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

		// 메시지 큐에 삽입
		EnterCriticalSection(&ThreadPool::rqcs);
		ThreadPool::recvQueue.push(buf);
		LeaveCriticalSection(&ThreadPool::rqcs);

		// 받은 데이터 출력
		printf("[TCP/%s:%d] %c, %d, %d, %d\n", inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port), buf.msgId, buf.lParam, buf.mParam, buf.rParam);

		ZeroMemory((int*)&buf, sizeof(buf));

		//WaitForSingleObject(ThreadPool::sqevents[id - 2], INFINITE);
		//if (!ThreadPool::sendQueues[id - 2]->empty())
		//{
		//	EnterCriticalSection(&ThreadPool::sqcss[id - 2]);
		//	buf = ThreadPool::sendQueues[id - 2]->front();
		//	ThreadPool::sendQueues[id - 2]->pop();
		//	LeaveCriticalSection(&ThreadPool::sqcss[id - 2]);
		//
		//	// 데이터 보내기
		//	retval = send(client_sock, (char*)&buf, sizeof(Message), 0);
		//	if (retval == SOCKET_ERROR) {
		//		err_display((char*)"send()");
		//		break;
		//	}
		//}
	}

	// closesocket()
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	// 종료된 클라이언트의 메시지 큐와 스레드 제거하는 메시지 넣자...

	return 0;
}

