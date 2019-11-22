#pragma once

class ThreadPool;

class MessagingThread : public Thread
{
public:
	SOCKET clientSock;
	bool isReady;

public:
	MessagingThread(int tId, LPVOID fParam);
	~MessagingThread();

};

DWORD WINAPI Messenger(LPVOID arg);
