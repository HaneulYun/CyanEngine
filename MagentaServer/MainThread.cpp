#include "pch.h"
#include "MainThread.h"

MainThread::MainThread(int tId, LPVOID fParam)
	: Thread(tId, Calculate, fParam)
{

}

MainThread::~MainThread()
{

}

DWORD WINAPI Calculate(LPVOID arg)	// �ӽ� �Լ� �̸�
{
	Message result;

	while (1) {
		//if (Scene::gameState == Runtime)
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

			// �ӽ�(�׽�Ʈ��)
			switch (curMessage.msgId)
			{
			case ADD:
				result.msgId = 'E';
				result.lParam = curMessage.lParam + curMessage.mParam + curMessage.rParam;
				result.mParam = curMessage.lParam + curMessage.mParam + curMessage.rParam;
				result.rParam = curMessage.lParam + curMessage.mParam + curMessage.rParam;
				printf("%d\n", result.lParam);

				EnterCriticalSection(&ThreadPool::sqcs);
				ThreadPool::sendQueue.push(result);
				LeaveCriticalSection(&ThreadPool::sqcs);
				break;

			case SUB:
				result.msgId = 'E';
				result.lParam = curMessage.lParam - curMessage.mParam - curMessage.rParam;
				result.mParam = curMessage.lParam - curMessage.mParam - curMessage.rParam;
				result.rParam = curMessage.lParam - curMessage.mParam - curMessage.rParam;
				printf("%d\n", result.lParam);

				EnterCriticalSection(&ThreadPool::sqcs);
				ThreadPool::sendQueue.push(result);
				LeaveCriticalSection(&ThreadPool::sqcs);
				break;

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