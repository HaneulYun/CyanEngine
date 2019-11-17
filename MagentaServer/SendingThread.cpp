#include "SendingThread.h"
#include "ThreadPool.h"
#include "PrintErrors.h"
#include "Scene.h"

CRITICAL_SECTION ThreadPool::sqcs;
queue<Message> ThreadPool::sendQueue;

SendingThread::SendingThread(int tId, LPVOID fParam)
	:Thread(tId, Sender, (LPVOID)fParam)
{

}

SendingThread::~SendingThread()
{

}

DWORD WINAPI Sender(LPVOID arg)
{
	int retval;

	while (1)
	{
		if(!ThreadPool::sendQueue.empty()){
			EnterCriticalSection(&ThreadPool::sqcs);
			Message curMessage = ThreadPool::sendQueue.front();
			ThreadPool::sendQueue.pop();

			for (int i = 0; i < ThreadPool::clients.size(); ++i)
			{
				if (ThreadPool::clients[i]->isWorking) {
					retval = send(ThreadPool::clients[i]->clientSock, (char*)&curMessage, sizeof(Message), 0);
					if (retval == SOCKET_ERROR) {
						err_display((char*)"send()");
						break;
					}
				}
			}

			if (curMessage.msgId == MESSAGE_GAME_START)
				Scene::gameState = Runtime;

			LeaveCriticalSection(&ThreadPool::sqcs);
		}
	}

	return 0;
}