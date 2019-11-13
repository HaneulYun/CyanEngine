#include "ConnectingThread.h"
#include "ThreadPool.h"

ConnectingThread::ConnectingThread(int tId, LPVOID fParam)
	: Thread(tId, ThreadPool::Connection, fParam)
{

}

ConnectingThread::~ConnectingThread()
{

}