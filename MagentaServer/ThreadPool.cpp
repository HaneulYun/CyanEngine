#include "pch.h"
//#include "ThreadPool.h"

MainThread* ThreadPool::mainThread;
ConnectingThread* ThreadPool::connThread;
SendingThread* ThreadPool::sendThread;
vector<MessagingThread*> ThreadPool::clients;

queue<Message> ThreadPool::recvQueue;
CRITICAL_SECTION ThreadPool::rqcs;
CRITICAL_SECTION ThreadPool::sqcs;
queue<Message> ThreadPool::sendQueue;

int ThreadPool::maxClients = 3;
int ThreadPool::nClients = 0;
SOCKET* ThreadPool::listenSock;

GameObject* ThreadPool::bulletGenerator;

ThreadPool::ThreadPool()
{
	InitializeCriticalSection(&rqcs);
	InitializeCriticalSection(&sqcs);

	mainThread = new MainThread(0, 0);
}

ThreadPool::~ThreadPool()
{
	for (MessagingThread* c : clients)
		delete c;
	delete sendThread;
	delete connThread;
	delete mainThread;

	DeleteCriticalSection(&sqcs);
	DeleteCriticalSection(&rqcs);
}

void ThreadPool::setConnectingThread(SOCKET* s)
{
	listenSock = s;
	connThread = new ConnectingThread(1, s);
	sendThread = new SendingThread(2, 0);
}

Message ThreadPool::curConnectedClients()
{
	Message msg;
	msg.msgId = MESSAGE_CONNECTED_IDS;
	msg.lParam = 0;
	msg.mParam = 0;
	msg.rParam = 0;

	for (int i = 0; i < clients.size(); ++i)
	{
		if (clients[i]->isWorking)
		{
			if (clients[i]->id == 3)
				msg.lParam = 1;
			else if (clients[i]->id == 4)
				msg.mParam = 1;
			else if (clients[i]->id == 5)
				msg.rParam = 1;
		}
	}

	return msg;
}

bool ThreadPool::isAllClientsReady()
{
	//if (clients.size() < maxClients)
	//	return false;
	
	for (int i = 0; i < clients.size(); ++i)
	{
		if (!clients[i]->isReady)
			return false;
	}

	return true;
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

			// 접속한 클라이언트 정보 출력
			printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
				inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

			clients.push_back(new MessagingThread(3 + nClients++, (LPVOID)client_sock));
			if (clients.back()->handle == NULL) { closesocket(client_sock); }
			//else { CloseHandle(threads.back()->handle); }

			// 너의 아이디를 반환~!
			Message clientsInform;
			clientsInform.msgId = MESSAGE_YOUR_ID;
			clientsInform.lParam = ThreadPool::clients.back()->id - 3;
			clientsInform.mParam = ThreadPool::clients.back()->id - 3;
			clientsInform.rParam = ThreadPool::clients.back()->id - 3;
			retval = send(ThreadPool::clients.back()->clientSock, (char*)&clientsInform, sizeof(Message), 0);
			if (retval == SOCKET_ERROR) {
				err_display((char*)"send()");
				break;
			}

			Message sendMsg = curConnectedClients();
			EnterCriticalSection(&sqcs);
			sendQueue.push(sendMsg);
			LeaveCriticalSection(&sqcs);
		}
		// 빈 아이디 찾아서 스레드 재사용하기
		else if(nClients == maxClients)
		{
			//Messenger 함수의 arg값을 바꾸어 스레드를 재사용할 수 있게 해보자
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

				// 접속한 클라이언트 정보 출력
				printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
					inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

				clients[index]->clientSock = client_sock;
				clients[index]->isWorking = true;
				if (clients[index]->handle == NULL) { closesocket(client_sock); }
				//else { CloseHandle(threads.back()->handle); }

				// 너의 아이디를 반환~!
				Message clientsInform;
				clientsInform.msgId = MESSAGE_YOUR_ID;
				clientsInform.lParam = ThreadPool::clients[index]->id - 3;
				clientsInform.mParam = ThreadPool::clients[index]->id - 3;
				clientsInform.rParam = ThreadPool::clients[index]->id - 3;
				retval = send(ThreadPool::clients[index]->clientSock, (char*)&clientsInform, sizeof(Message), 0);
				if (retval == SOCKET_ERROR) {
					err_display((char*)"send()");
					break;
				}

				Message sendMsg = curConnectedClients();
				EnterCriticalSection(&sqcs);
				sendQueue.push(sendMsg);
				LeaveCriticalSection(&sqcs);
			}
		}
	}
	
	closesocket(*(SOCKET*)listen_sock);

	return 0;
}

int ThreadPool::getRestedThread()
{
	for (int i = 0; i < clients.size(); ++i)
	{
		if (!clients[i]->isWorking)
		{
			return i;
		}
	}
	return -1;
}