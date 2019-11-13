#pragma once
#include "Thread.h"

class ConnectingThread : public Thread
{
public:
	ConnectingThread(int tId, LPVOID fParam);
	~ConnectingThread();
};