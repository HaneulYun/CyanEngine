#pragma once
#include <windows.h>
#include <queue>
#include "Message.h"
#include "Thread.h"

class MessagingThread : public Thread
{
private:
	std::queue<Message> sendQueue;

public:
	MessagingThread(int tId, LPVOID fParam);
	~MessagingThread();

};

DWORD WINAPI Messenger(LPVOID arg);
