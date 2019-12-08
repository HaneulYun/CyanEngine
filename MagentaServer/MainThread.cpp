#include "pch.h"
#include "MainThread.h"
#include "SceneManager.h"

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
	int type;

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
					PushToSendQueue(result, curMessage, MESSAGE_GAME_START);
				break;

			case MESSAGE_REQUEST_BULLET_CREATION:
				type = curMessage.msgId - MESSAGE_REQUEST_BULLET_CREATION_STRAIGHT;
				curMessage.lParam = SceneManager::scenemanager->CreateBulletAndGetObjID(type);
				PushToSendQueue(result, curMessage, MESSAGE_CREATE_BULLET);
				break;

			case MESSAGE_REQUEST_BULLET_CREATION_STRAIGHT:
				type = curMessage.msgId - MESSAGE_REQUEST_BULLET_CREATION_STRAIGHT;
				curMessage.lParam = SceneManager::scenemanager->CreateBulletAndGetObjID(type);
				PushToSendQueue(result, curMessage, MESSAGE_CREATE_BULLET_STRAIGHT);
				break;

			case MESSAGE_REQUEST_BULLET_CREATION_CANNON:
				type = curMessage.msgId - MESSAGE_REQUEST_BULLET_CREATION_STRAIGHT;
				curMessage.lParam = SceneManager::scenemanager->CreateBulletAndGetObjID(type);
				PushToSendQueue(result, curMessage, MESSAGE_CREATE_BULLET_CANNON);
				break;

			case MESSAGE_REQUEST_BULLET_CREATION_SHARP:
				type = curMessage.msgId - MESSAGE_REQUEST_BULLET_CREATION_STRAIGHT;
				curMessage.lParam = SceneManager::scenemanager->CreateBulletAndGetObjID(type);
				PushToSendQueue(result, curMessage, MESSAGE_CREATE_BULLET_SHARP);
				break;

			case MESSAGE_REQUEST_BULLET_CREATION_LASER:
				type = curMessage.msgId - MESSAGE_REQUEST_BULLET_CREATION_STRAIGHT;
				curMessage.lParam = SceneManager::scenemanager->CreateBulletAndGetObjID(type);
				PushToSendQueue(result, curMessage, MESSAGE_CREATE_BULLET_LASER);
				break;

			case MESSAGE_REQUEST_BULLET_CREATION_GUIDED:
				type = curMessage.msgId - MESSAGE_REQUEST_BULLET_CREATION_STRAIGHT;
				curMessage.lParam = SceneManager::scenemanager->CreateBulletAndGetObjID(type);
				PushToSendQueue(result, curMessage, MESSAGE_CREATE_BULLET_GUIDED);
				break;

			case MESSAGE_NOTIFY_COLLISION_BULLET_AND_ENEMY:
				SceneManager::scenemanager->DeleteObjectID(curMessage.lParam);
				PushToSendQueue(result, curMessage, MESSAGE_NOTIFY_COLLISION_BULLET_AND_ENEMY);
				if (SceneManager::scenemanager->AddDamageToEnemy((int)curMessage.mParam, (int)curMessage.rParam))
				{
					curMessage.lParam = curMessage.mParam;
					SceneManager::scenemanager->DeleteObjectID(curMessage.lParam);
					PushToSendQueue(result, curMessage, MESSAGE_DELETE_ENEMY);
				}
				break;
			}
		}
	}
	return 0;
}

void MainThread::PushToSendQueue(Message& result, Message& msg, unsigned char id)
{
	result.msgId = id;
	result.lParam = msg.lParam;
	result.mParam = msg.mParam;
	result.rParam = msg.rParam;
	EnterCriticalSection(&ThreadPool::sqcs);
	ThreadPool::sendQueue.push(result);
	LeaveCriticalSection(&ThreadPool::sqcs);
}

