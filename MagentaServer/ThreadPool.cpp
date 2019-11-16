#include "ThreadPool.h"
#include "PrintErrors.h"
#include "Globals.h"

MainThread* ThreadPool::mainThread;
ConnectingThread* ThreadPool::connThread;
vector<MessagingThread*> ThreadPool::clients;

int ThreadPool::maxClients = 3;
int ThreadPool::nClients = 0;
SOCKET* ThreadPool::listenSock;

ThreadPool::ThreadPool()
{
	InitializeCriticalSection(&rqcs);

	mainThread = new MainThread(0, 0);
}

ThreadPool::~ThreadPool()
{
	for (MessagingThread* c : clients)
		delete c;
	delete connThread;
	delete mainThread;

	DeleteCriticalSection(&rqcs);
}

void ThreadPool::setConnectingThread(SOCKET* s)
{
	listenSock = s;
	connThread = new ConnectingThread(1, s);
}

DWORD WINAPI ThreadPool::Connection(LPVOID listen_sock)
{
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;

	while (1)
	{
		if (nClients < maxClients)
		{
			// accept()
			addrlen = sizeof(clientaddr);
			client_sock = accept(*(SOCKET*)listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
			if (client_sock == INVALID_SOCKET) {
				err_display((char*)"accept()");
				break;
			}

			// ������ Ŭ���̾�Ʈ ���� ���
			printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
				inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

			clients.push_back(new MessagingThread(2 + nClients++, (LPVOID)client_sock));
			if (clients.back()->handle == NULL) { closesocket(client_sock); }
			//else { CloseHandle(threads.back()->handle); }
		}
		// �� ���̵� ã�Ƽ� ������ �����ϱ�
		{
			//Messenger �Լ��� arg���� �ٲپ� �����带 ������ �� �ְ� �غ���
		}
	}
	
	closesocket(*(SOCKET*)listen_sock);

	return 0;
}

int ThreadPool::getEmptyThread()
{
	return 0;
}