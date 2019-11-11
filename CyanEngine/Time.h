#pragma once

const ULONG MAX_SAMPLE_COUNT = 50;

class Time : public Singleton<Time>
{
private:
	bool m_bHardwareHasPerformanceCounter;
	float m_fTimeScale;
	union {
		float m_fTimeElapsed{ 0 };
	};
	__int64 m_nCurrentTime{ 0 };
	__int64 m_nLastTime;
	__int64 m_nPerformanceFrequency;

	float m_fFrameTime[MAX_SAMPLE_COUNT];
	ULONG m_nSampleCount{ 0 };

	unsigned long m_nCurrentFrameRate{ 0 };
	unsigned long m_nFramesPerSecond{ 0 };
	float m_fFPSTimeElapsed{ 0.0f };

public:
	static float deltaTime;

public:
	Time();
	~Time();

	void Tick(float fLockFPS = 0.0f);
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
	float GetTimeElapsed();
	void Reset() {}; /* need definition */
};

