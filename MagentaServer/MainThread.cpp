#include <stdio.h>
#include "MainThread.h"
#include "ThreadPool.h"

//queue<Message> ThreadPool::recvQueue;
//CRITICAL_SECTION ThreadPool::rqcs;

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
			LeaveCriticalSection(&ThreadPool::rqcs);

			if(curMessage.msgId == ADD)
				printf("%d\n", curMessage.lParam + curMessage.mParam + curMessage.rParam);
			else if(curMessage.msgId == SUB)
				printf("%d\n", curMessage.lParam - curMessage.mParam - curMessage.rParam);
		}
	}
	return 0;
}