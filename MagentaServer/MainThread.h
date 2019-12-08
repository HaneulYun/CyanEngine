#pragma once


class MainThread : public Thread
{
public:
	MainThread(int tId, LPVOID fParam);
	~MainThread();

	static DWORD WINAPI Calculate(LPVOID arg);

	static void PushToSendQueue(Message& result, Message& msg, unsigned char id);
};