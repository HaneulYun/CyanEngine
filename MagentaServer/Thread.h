#pragma once

class Thread
{
public:
	int id;
	HANDLE handle;

	bool isWorking = true;

public:
	Thread();
	Thread(int tId, LPTHREAD_START_ROUTINE func, LPVOID fParam);
	~Thread();

};