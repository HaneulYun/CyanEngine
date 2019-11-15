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
	Message buf;	// �ӽ� ����

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

		// ���� ������ ���
		//((char*)buf)[retval] = '\0';
		printf("[TCP/%s:%d] %c, %d, %d, %d\n", inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port), buf.msgId, buf.lParam, buf.mParam, buf.rParam);

		// ������ ������
		retval = send(client_sock, (char*)&buf, sizeof(Message), 0);	
		if (retval == SOCKET_ERROR) {
			err_display((char*)"send()");
			break;
		}

		printf("���� ���Ҵ�!\n");	// �ӽ� ��º�
	}

	// closesocket()
	closesocket(client_sock);
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	// ����� Ŭ���̾�Ʈ�� �޽��� ť�� ������ �����ϴ� �޽��� ����...

	return 0;
}

