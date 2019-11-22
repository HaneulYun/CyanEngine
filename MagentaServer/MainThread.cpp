#include "pch.h"
#include "MainThread.h"

extern MagentaFW gMagentaFW;

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
		gMagentaFW.FrameAdvance();

		while (!ThreadPool::recvQueue.empty())
		{
			EnterCriticalSection(&ThreadPool::rqcs);
			Message curMessage = ThreadPool::recvQueue.front();
			ThreadPool::recvQueue.pop();
			LeaveCriticalSection(&ThreadPool::rqcs);

			// 임시(테스트용)
			switch (curMessage.msgId)
			{
			case MESSAGE_READY:
				ThreadPool::clients[curMessage.lParam]->isReady = true;
				if (ThreadPool::isAllClientsReady())
				{
					result.msgId = MESSAGE_GAME_START;
					result.lParam = 0;
					result.mParam = 0;
					result.rParam = 0;
					EnterCriticalSection(&ThreadPool::sqcs);
					ThreadPool::sendQueue.push(result);
					LeaveCriticalSection(&ThreadPool::sqcs);
				}
				break;
			case MESSAGE_REQUEST_BULLET_CREATION:
				result.msgId = MESSAGE_CREATE_BULLET;
				result.lParam = curMessage.lParam;
				result.mParam = curMessage.mParam;
				result.rParam = curMessage.rParam;
				break;
			}
		}

	}
	return 0;
}