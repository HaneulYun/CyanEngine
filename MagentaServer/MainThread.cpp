#include "pch.h"
#include "MainThread.h"

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
		//if (GameScene::gameState == Runtime)
		//{
		//	Time::Instance()->Tick();
		//	Scene::star->Update();
		//}

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
			}
		}

	}
	return 0;
}