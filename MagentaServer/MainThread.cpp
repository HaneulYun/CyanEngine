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
	while (1) {
		while (!ThreadPool::recvQueue.empty())
		{
			EnterCriticalSection(&ThreadPool::rqcs);
			Message curMessage = ThreadPool::recvQueue.front();
			ThreadPool::recvQueue.pop();

			Message result;
			// 임시(테스트용)
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

			//for (int i = 0; i < ThreadPool::getNThreads() - 2; ++i)
			//{
			//	EnterCriticalSection(&ThreadPool::sqcss[i]);
			//	ThreadPool::sendQueues[i]->push(result);
			//	LeaveCriticalSection(&ThreadPool::sqcss[i]);
			//	SetEvent(ThreadPool::sqevents[i]);
			//}
			//
			//if (!ThreadPool::sendQueues[id - 2]->empty())
			//{
			//	EnterCriticalSection(&ThreadPool::sqcss[id - 2]);
			//	buf = ThreadPool::sendQueues[id - 2]->front();
			//	ThreadPool::sendQueues[id - 2]->pop();
			//	LeaveCriticalSection(&ThreadPool::sqcss[id - 2]);
			//
			//	// 데이터 보내기
			//	retval = send(client_sock, (char*)&buf, sizeof(Message), 0);
			//	if (retval == SOCKET_ERROR) {
			//		err_display((char*)"send()");
			//		break;
			//	}
			//}

			int retval;
			for (int i = 0; i < ThreadPool::getNThreads() - 2; ++i)
			{
				retval = send(ThreadPool::clientSock[i], (char*)&result, sizeof(Message), 0);
				if (retval == SOCKET_ERROR) {
					//err_display((char*)"send()");
					break;
				}
			}
		}

	}
	return 0;
}