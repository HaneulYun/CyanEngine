#include "pch.h"
#include "MessagingThread.h"

MessagingThread::MessagingThread(int tId, LPVOID fParam)
	: Thread(tId, Messenger, (LPVOID)tId)
{
	clientSock = (SOCKET)fParam;
	isReady = false;
}

MessagingThread::~MessagingThread()
{
}

DWORD WINAPI Messenger(LPVOID arg)
{
	while (1) {
		if (ThreadPool::clients[(int)arg - 3]->isWorking)
		{
			// id�� arg�� �޴´ٸ�..?
			MessagingThread* msgt = ThreadPool::clients[(int)arg - 3];

			SOCKET client_sock = msgt->clientSock;
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

				// �޽��� ť�� ����
				EnterCriticalSection(&ThreadPool::rqcs);
				ThreadPool::recvQueue.push(buf);
				LeaveCriticalSection(&ThreadPool::rqcs);

				// ���� ������ ���
				printf("[TCP/%s:%d] %c, %d, %d, %d\n", inet_ntoa(clientaddr.sin_addr),
					ntohs(clientaddr.sin_port), buf.msgId, buf.lParam, buf.mParam, buf.rParam);

				ZeroMemory((int*)&buf, sizeof(buf));
			}

			// closesocket()
			closesocket(client_sock);
			printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
				inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			msgt->isWorking = false;
			msgt->isReady = false;

			Message sendMsg = ThreadPool::curConnectedClients();
			EnterCriticalSection(&ThreadPool::sqcs);
			ThreadPool::sendQueue.push(sendMsg);
			LeaveCriticalSection(&ThreadPool::sqcs);
		}
	}
	return 0;
}
