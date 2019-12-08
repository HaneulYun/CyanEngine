#include "pch.h"
#include "MainThread.h"

extern MagentaFW gMagentaFW;

MainThread::MainThread(int tId, LPVOID fParam)
	: Thread(tId, MainThread::Calculate, fParam)
{

}

MainThread::~MainThread()
{

}

DWORD WINAPI MainThread::Calculate(LPVOID arg)	// 임시 함수 이름
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
					RequestBulletCreation(result, curMessage, MESSAGE_GAME_START);
				break;
			case MESSAGE_REQUEST_BULLET_CREATION:
				RequestBulletCreation(result, curMessage, MESSAGE_CREATE_BULLET);
				//ThreadPool::bulletGenerator->GetComponent<BulletGenerator>()->CreateBulletAndGetObjID(0);
				break;
			case MESSAGE_REQUEST_BULLET_CREATION_STRAIGHT:
				RequestBulletCreation(result, curMessage, MESSAGE_CREATE_BULLET_STRAIGHT);
				//ThreadPool::bulletGenerator->GetComponent<BulletGenerator>()->CreateBulletAndGetObjID(0);
				break;
			case MESSAGE_REQUEST_BULLET_CREATION_CANNON:
				RequestBulletCreation(result, curMessage, MESSAGE_CREATE_BULLET_CANNON);
				//ThreadPool::bulletGenerator->GetComponent<BulletGenerator>()->CreateBulletAndGetObjID(1);
				break;
			case MESSAGE_REQUEST_BULLET_CREATION_SHARP:
				RequestBulletCreation(result, curMessage, MESSAGE_CREATE_BULLET_SHARP);
				//ThreadPool::bulletGenerator->GetComponent<BulletGenerator>()->CreateBulletAndGetObjID(2);
				break;
			case MESSAGE_REQUEST_BULLET_CREATION_LASER:
				RequestBulletCreation(result, curMessage, MESSAGE_CREATE_BULLET_LASER);
				//ThreadPool::bulletGenerator->GetComponent<BulletGenerator>()->CreateBulletAndGetObjID(3);
				break;
			case MESSAGE_REQUEST_BULLET_CREATION_GUIDED:
				RequestBulletCreation(result, curMessage, MESSAGE_CREATE_BULLET_GUIDED);
				//ThreadPool::bulletGenerator->GetComponent<BulletGenerator>()->CreateBulletAndGetObjID(4);
				break;

			case MESSAGE_NOTIFY_COLLISION_BULLET_AND_ENEMY:
				RequestBulletCreation(result, curMessage, MESSAGE_NOTIFY_COLLISION_BULLET_AND_ENEMY);
				break;
			}
		}
	}
	return 0;
}

void MainThread::RequestBulletCreation(Message& result, Message& msg, unsigned char id)
{
	result.msgId = id;
	result.lParam = msg.lParam;
	result.mParam = msg.mParam;
	result.rParam = msg.rParam;
	EnterCriticalSection(&ThreadPool::sqcs);
	ThreadPool::sendQueue.push(result);
	LeaveCriticalSection(&ThreadPool::sqcs);
}

