#pragma once
#include <windows.h>
#include "Thread.h"

class MessagingThread : public Thread
{

public:
	MessagingThread(int tId, LPVOID fParam);
	~MessagingThread();

};

DWORD WINAPI Messenger(LPVOID arg);
