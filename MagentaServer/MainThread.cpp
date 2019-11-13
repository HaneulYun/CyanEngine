#include <stdio.h>
#include "MainThread.h"

MainThread::MainThread(int tId, LPVOID fParam)
	: Thread(tId, Calculate, fParam)
{

}

MainThread::~MainThread()
{

}

DWORD WINAPI Calculate(LPVOID arg)	// 임시 함수 이름
{
	int i = 0;

	while (1)
		printf("main: %d\n", ++i);

	return 0;
}