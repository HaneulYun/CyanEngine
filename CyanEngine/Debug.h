#pragma once

class Debug : public Singleton<Debug>
{
public:
	Debug();
	~Debug();

	static void Log(const char* log)
	{
		OutputDebugStringA(log);
	}
};