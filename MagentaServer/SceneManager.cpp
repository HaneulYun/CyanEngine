#include "pch.h"
#include "scripts.h"
#include "SceneManager.h"

void SceneManager::Start()
{
	scenemanager = this;
}

void SceneManager::Update()
{
	static float time = 0.f;
	time += Time::deltaTime;
	if (gameState == START)
	{
		standardAngle += speedRotating * Time::deltaTime;
		if (star->GetComponent<Damageable>()->isDead())
		{
			gameState = END;
			Message sendMsg;
			sendMsg.msgId = MESSAGE_GAME_END;
			EnterCriticalSection(&ThreadPool::sqcs);
			ThreadPool::sendQueue.push(sendMsg);
			LeaveCriticalSection(&ThreadPool::sqcs);
			printf("³¡\n");
		}
	}
}

void SceneManager::CreatePlayer(int id)
{

}

void SceneManager::StartGame()
{
	gameState = START;
}

float SceneManager::getStandardAngle()
{
	return standardAngle;
}

int SceneManager::CreateBulletAndGetObjID(int type)
{
	return bulletGenerator->CreateBulletAndGetObjID(type);
}