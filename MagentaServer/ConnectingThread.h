#pragma once

class ThreadPool;

class ConnectingThread : public Thread
{
public:
	ConnectingThread(int tId, LPVOID fParam);
	~ConnectingThread();
};