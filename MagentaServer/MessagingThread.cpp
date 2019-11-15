#include <stdio.h>
#include <queue>
#include "ThreadPool.h"
#include "MessagingThread.h"
#include "Globals.h"
#include "PrintErrors.h"

queue<Message> ThreadPool::recvQueue;
CRITICAL_SECTION ThreadPool::rqcs;

vector<queue<Message>*> ThreadPool::sendQueues;

MessagingThread::MessagingThread(int tId, LPVOID fParam)
	: Thread(tId, Messenger, fParam)
{
	ThreadPool::sendQueues.push_back(&sendQueue);
	ThreadPool::sqcss.push_back(CRITICAL_SECTION());
	ThreadPool::sqevents.push_back(HANDLE(CreateEvent(NULL, FALSE, FALSE, NULL)));
	if (ThreadPool::sqevents[id - 2] == NULL);
	InitializeCriticalSection(&ThreadPool::sqcss[id - 2]);
}

MessagingThread::~MessagingThread()
{
	DeleteCriticalSection(&ThreadPool::sqcss[id - 2]);
}

DWORD WINAPI Messenger(LPVOID arg)
{
	int id = ThreadPool::getNThreads() - 1;
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	Message buf;	// �ӽ� ����

	// �̺�Ʈ�� ���� ����
	DWORD rEvent;

	// Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);
	
	while (1) {
		// ������ �ޱ�
		retval = recv(client_sock, (char*)&buf, sizeof(Message), 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"recv()");
			break;
		}
		else if (retval == 0)
			break;

		// �޽��� ť�� ����
		EnterCriticalSection(&ThreadPool::rqcs);
		ThreadPool::recvQueue.push(buf);
		LeaveCriticalSection(&ThreadPool::rqcs);

		// ���� ������ ���
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
		//	// ������ ������
		//	retval = send(client_sock, (char*)&buf, sizeof(Message), 0);
		//	if (retval == SOCKET_ERROR) {
		//		err_display((char*)"send()");
		//		break;
		//	}
		//}
	}

	// closesocket()
	closesocket(client_sock);
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	// ����� Ŭ���̾�Ʈ�� �޽��� ť�� ������ �����ϴ� �޽��� ����...

	return 0;
}

