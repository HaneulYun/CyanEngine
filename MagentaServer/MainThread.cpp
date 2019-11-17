#include <stdio.h>
#include "MainThread.h"
#include "ThreadPool.h"

MainThread::MainThread(int tId, LPVOID fParam)
	: Thread(tId, Calculate, fParam)
{

}

MainThread::~MainThread()
{

}

DWORD WINAPI Calculate(LPVOID arg)	// 임시 함수 이름
{
	Message result;

	while (1) {
		while (!ThreadPool::recvQueue.empty())
		{
			EnterCriticalSection(&ThreadPool::rqcs);
			Message curMessage = ThreadPool::recvQueue.front();
			ThreadPool::recvQueue.pop();
			LeaveCriticalSection(&ThreadPool::rqcs);

			// 임시(테스트용)
			switch (curMessage.msgId)
			{
			case ADD:
				result.msgId = 'E';
				result.lParam = curMessage.lParam + curMessage.mParam + curMessage.rParam;
				result.mParam = curMessage.lParam + curMessage.mParam + curMessage.rParam;
				result.rParam = curMessage.lParam + curMessage.mParam + curMessage.rParam;
				printf("%d\n", result.lParam);
				break;

			case SUB:
				result.msgId = 'E';
				result.lParam = curMessage.lParam - curMessage.mParam - curMessage.rParam;
				result.mParam = curMessage.lParam - curMessage.mParam - curMessage.rParam;
				result.rParam = curMessage.lParam - curMessage.mParam - curMessage.rParam;
				printf("%d\n", result.lParam);
				break;
			}

			EnterCriticalSection(&ThreadPool::sqcs);
			ThreadPool::sendQueue.push(result);
			LeaveCriticalSection(&ThreadPool::sqcs);
		}

	}
	return 0;
}