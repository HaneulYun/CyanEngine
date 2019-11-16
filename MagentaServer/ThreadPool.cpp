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

			// 접속한 클라이언트 정보 출력
			printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
				inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));


			threads.push_back(new MessagingThread(nThreads++, (LPVOID)client_sock));
			clientSock.push_back(client_sock);
			if (threads.back()->handle == NULL) { closesocket(client_sock); }
			//else { CloseHandle(threads.back()->handle); }
		}
		// 빈 아이디 찾아서 스레드 재사용하기
		{
			//Messenger 함수의 arg값을 바꾸어 스레드를 재사용할 수 있게 해보자
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