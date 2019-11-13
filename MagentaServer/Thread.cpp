#include "Thread.h"

Thread::Thread()
{
	id = -1;
	handle = NULL;
}

Thread::Thread(int tId, LPTHREAD_START_ROUTINE func, LPVOID fParam)
{
	id = tId;
	handle = CreateThread(NULL, 0, func, fParam, 0, NULL);
	if (handle != NULL) { CloseHandle(handle); }
}

Thread::~Thread()
{

}