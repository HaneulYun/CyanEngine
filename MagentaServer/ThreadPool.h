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
public:
	static MainThread *mainThread;	// 0 
	static ConnectingThread *connThread;	// 1
	static vector<MessagingThread*> clients;	// 2 ~ maxClients

	static int nClients;
	static int maxClients;
	static SOCKET *listenSock;

	static CRITICAL_SECTION rqcs;
	static queue<Message> recvQueue;

public:
	ThreadPool();
	~ThreadPool();

	static void setConnectingThread(SOCKET *s);
	static DWORD WINAPI Connection(LPVOID listen_sock);
	int getEmptyThread();
}; 


