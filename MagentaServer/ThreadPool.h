#pragma once
#include <vector>
#include "Singleton.h"
#include "MainThread.h"
#include "ConnectingThread.h"
#include "MessagingThread.h"

using namespace std;

class ThreadPool : public Singleton<ThreadPool>
{
private:
	static vector<Thread*> threads;
	static int nThreads;
	static int maxThreads;

public:
	ThreadPool(int num, SOCKET* listen_sock);
	~ThreadPool();

	static DWORD WINAPI Connection(LPVOID listen_sock);

	int getEmptyThread();
}; 


