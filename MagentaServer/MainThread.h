#pragma once

DWORD WINAPI Calculate(LPVOID arg);

class MainThread : public Thread
{
public:
	MainThread(int tId, LPVOID fParam);
	~MainThread();
};