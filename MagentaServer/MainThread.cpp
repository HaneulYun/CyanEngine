#include <stdio.h>
#include "MainThread.h"
#include "ThreadPool.h"
#include "PrintErrors.h"

MainThread::MainThread(int tId, LPVOID fParam)
	: Thread(tId, Calculate, fParam)
{

}

MainThread::~MainThread()
{

}

DWORD WINAPI Calculate(LPVOID arg)	// �ӽ� �Լ� �̸�
{
	while (1) {
		while (!ThreadPool::recvQueue.empty())
		{
			EnterCriticalSection(&ThreadPool::rqcs);
			Message curMessage = ThreadPool::recvQueue.front();
			ThreadPool::recvQueue.pop();

			Message result;
			// �ӽ�(�׽�Ʈ��)
			if (curMessage.msgId == ADD)
			{
				result.msgId = 'E';
				result.lParam = curMessage.lParam + curMessage.mParam + curMessage.rParam;
				result.mParam = curMessage.lParam + curMessage.mParam + curMessage.rParam;
				result.rParam = curMessage.lParam + curMessage.mParam + curMessage.rParam;
				printf("%d\n", result.lParam);
			}
			else if(curMessage.msgId == SUB)
			{
				result.msgId = 'E';
				result.lParam = curMessage.lParam - curMessage.mParam - curMessage.rParam;
				result.mParam = curMessage.lParam - curMessage.mParam - curMessage.rParam;
				result.rParam = curMessage.lParam - curMessage.mParam - curMessage.rParam;
				printf("%d\n", result.lParam);
			}
			LeaveCriticalSection(&ThreadPool::rqcs);

			int retval;
			for (int i = 0; i < ThreadPool::clients.size(); ++i)
			{
				if (ThreadPool::clients[i]->isWorking) {
					retval = send(ThreadPool::clients[i]->clientSock, (char*)&result, sizeof(Message), 0);
					if (retval == SOCKET_ERROR) {
						err_display((char*)"send()");
						break;
					}
				}
			}
		}

	}
	return 0;
}