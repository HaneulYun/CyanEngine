#pragma once
#include <windows.h>
#include "Thread.h"

DWORD WINAPI Calculate(LPVOID arg);

class MainThread : public Thread
{
public:
	MainThread(int tId, LPVOID fParam);
	~MainThread();
};