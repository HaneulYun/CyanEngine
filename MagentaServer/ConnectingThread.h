#pragma once

class ConnectingThread : public Thread
{
public:
	ConnectingThread(int tId, LPVOID fParam);
	~ConnectingThread();
};