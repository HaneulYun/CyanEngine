#pragma once

class SendingThread : public Thread
{
public:
	SendingThread(int tId, LPVOID fParam);
	~SendingThread();

};

DWORD WINAPI Sender(LPVOID arg);