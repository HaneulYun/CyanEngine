#pragma once
#include <vector>
#include <queue>
#include "Singleton.h"
#include "MainThread.h"
#include "SendingThread.h"
#include "ConnectingThread.h"
#include "MessagingThread.h"
#include "Message.h"

using namespace std;

class ThreadPool : public Singleton<ThreadPool>
{
public:
	static MainThread *mainThread;	// 0
	static ConnectingThread* connThread;	// 1
	static SendingThread *sendThread;	// 2
	static vector<MessagingThread*> clients;	// 3 ~ maxClients

	static int nClients;
	static int maxClients;
	static SOCKET *listenSock;

	static CRITICAL_SECTION rqcs;
	static queue<Message> recvQueue;
	static CRITICAL_SECTION sqcs;
	static queue<Message> sendQueue;

public:
	ThreadPool();
	~ThreadPool();

	static void setConnectingThread(SOCKET *s);
	static DWORD WINAPI Connection(LPVOID listen_sock);
	static int getRestedThread();
	static Message curConnectedClients();
}; 


