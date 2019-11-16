#include <stdio.h>
#include <queue>
#include "ThreadPool.h"
#include "MessagingThread.h"
#include "Globals.h"
#include "PrintErrors.h"

queue<Message> ThreadPool::recvQueue;
CRITICAL_SECTION ThreadPool::rqcs;

MessagingThread::MessagingThread(int tId, LPVOID fParam)
	: Thread(tId, Messenger, (LPVOID)tId)
{
	clientSock = (SOCKET)fParam;
}

MessagingThread::~MessagingThread()
{
}

DWORD WINAPI Messenger(LPVOID arg)
{
	while (1) {
		if(ThreadPool::clients[(int)arg-2]->isWorking)
		{
			// id를 arg로 받는다면..?
			MessagingThread* msgt = ThreadPool::clients[(int)arg - 2];

			SOCKET client_sock = msgt->clientSock;
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

				// 메시지 큐에 삽입
				EnterCriticalSection(&ThreadPool::rqcs);
				ThreadPool::recvQueue.push(buf);
				LeaveCriticalSection(&ThreadPool::rqcs);

				// 받은 데이터 출력
				printf("[TCP/%s:%d] %c, %d, %d, %d\n", inet_ntoa(clientaddr.sin_addr),
					ntohs(clientaddr.sin_port), buf.msgId, buf.lParam, buf.mParam, buf.rParam);

				ZeroMemory((int*)&buf, sizeof(buf));
			}

			// closesocket()
			closesocket(client_sock);
			printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
				inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			msgt->isWorking = false;
		}
	}
	return 0;
}

