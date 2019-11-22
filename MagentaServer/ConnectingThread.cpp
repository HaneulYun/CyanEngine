#include "pch.h"
#include "ConnectingThread.h"

ConnectingThread::ConnectingThread(int tId, LPVOID fParam)
	: Thread(tId, ThreadPool::Connection, fParam)
{

}

ConnectingThread::~ConnectingThread()
{

}