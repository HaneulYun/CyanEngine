#include "ThreadPool.h"
#include "PrintErrors.h"
#include "Globals.h"

vector<Thread*> ThreadPool::threads;
int ThreadPool::maxThreads = 5;
int ThreadPool::nThreads;
vector<SOCKET> ThreadPool::clientSock;

ThreadPool::ThreadPool(int num, SOCKET* listen_sock)
{
	InitializeCriticalSection(&rqcs);

	if (maxThreads > 2)
		maxThreads = num;

	threads.push_back(new MainThread(nThreads++, 0));
	threads.push_back(new ConnectingThread(nThreads++, listen_sock));
}

ThreadPool::~ThreadPool()
{
	for (Thread* ths : threads)
		delete ths;

	DeleteCriticalSection(&rqcs);
}

DWORD WINAPI ThreadPool::Connection(LPVOID listen_sock)
{
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;

	while (1)
	{
		if (nThreads < maxThreads)
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


			threads.push_back(new MessagingThread(nThreads++, (LPVOID)client_sock));
			clientSock.push_back(client_sock);
			if (threads.back()->handle == NULL) { closesocket(client_sock); }
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

int ThreadPool::getNThreads()
{
	return nThreads;
}