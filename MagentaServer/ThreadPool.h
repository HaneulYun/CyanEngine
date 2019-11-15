#pragma once
#include <vector>
#include <queue>
#include "Singleton.h"
#include "MainThread.h"
#include "ConnectingThread.h"
#include "MessagingThread.h"
#include "Message.h"

using namespace std;

class ThreadPool : public Singleton<ThreadPool>
{
private:
	static vector<Thread*> threads;
	static int nThreads;
	static int maxThreads;

public:
	static CRITICAL_SECTION rqcs;
	static queue<Message> recvQueue;
	static vector<SOCKET> clientSock;

public:
	ThreadPool(int num, SOCKET* listen_sock);
	~ThreadPool();

	static DWORD WINAPI Connection(LPVOID listen_sock);
	static int getNThreads();
	int getEmptyThread();
}; 


