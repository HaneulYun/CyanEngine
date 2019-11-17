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
	int retval;

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

			// ���� ���̵� ��ȯ~!
			Message clientsInform;
			clientsInform.msgId = MESSAGE_YOUR_ID;
			clientsInform.lParam = ThreadPool::clients.back()->id - 2;
			clientsInform.mParam = ThreadPool::clients.back()->id - 2;
			clientsInform.rParam = ThreadPool::clients.back()->id - 2;
			retval = send(ThreadPool::clients.back()->clientSock, (char*)&clientsInform, sizeof(Message), 0);
			if (retval == SOCKET_ERROR) {
				err_display((char*)"send()");
				break;
			}
		}
		// �� ���̵� ã�Ƽ� ������ �����ϱ�
		else if(nClients == maxClients)
		{
			//Messenger �Լ��� arg���� �ٲپ� �����带 ������ �� �ְ� �غ���
			int index = getRestedThread();

			if (index >= 0)
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

				clients[index]->clientSock = client_sock;
				clients[index]->isWorking = true;
				if (clients[index]->handle == NULL) { closesocket(client_sock); }
				//else { CloseHandle(threads.back()->handle); }

				// ���� ���̵� ��ȯ~!
				Message clientsInform;
				clientsInform.msgId = MESSAGE_YOUR_ID;
				clientsInform.lParam = ThreadPool::clients[index]->id - 2;
				clientsInform.mParam = ThreadPool::clients[index]->id - 2;
				clientsInform.rParam = ThreadPool::clients[index]->id - 2;
				retval = send(ThreadPool::clients[index]->clientSock, (char*)&clientsInform, sizeof(Message), 0);
				if (retval == SOCKET_ERROR) {
					err_display((char*)"send()");
					break;
				}
			}
		}
	}
	
	closesocket(*(SOCKET*)listen_sock);

	return 0;
}

int ThreadPool::getRestedThread()
{
	//if (clients.size() == 0)
	//	return 0;

	for (int i = 0; i < clients.size(); ++i)
	{
		if (!clients[i]->isWorking)
		{
			return i;
		}
	}
	return -1;
}