#pragma once
#include "pch.h"
#include "Time.h"

float Time::deltaTime = 0;

Time::Time()
{
	if (::QueryPerformanceFrequency((LARGE_INTEGER*)&m_nPerformanceFrequency))
	{
		m_bHardwareHasPerformanceCounter = TRUE;
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_nLastTime);
		m_fTimeScale = 1.0f / m_nPerformanceFrequency;
	}
	else
	{
		m_bHardwareHasPerformanceCounter = FALSE;
		m_nLastTime = ::GetTickCount64();
		m_fTimeScale = 0.001f;
	}
}
Time::~Time()
{
}

void Time::Tick(float fLockFPS)
{
	if (m_bHardwareHasPerformanceCounter)
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrentTime);
	}
	else
	{
		m_nCurrentTime = ::GetTickCount64();
	}

	float fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;

	if (fLockFPS > 0.0f)
	{
		while (fTimeElapsed < (1.0f / fLockFPS))
		{
			if (m_bHardwareHasPerformanceCounter)
			{
				::QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrentTime);
			}
			else
			{
				m_nCurrentTime = ::GetTickCount64();
			}
			fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;
		}
	}

	m_nLastTime = m_nCurrentTime;
	if (fabsf(fTimeElapsed - m_fTimeElapsed) < 1.0f)
	{
		::memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		m_fFrameTime[0] = fTimeElapsed;
		if (m_nSampleCount < MAX_SAMPLE_COUNT) m_nSampleCount++;
	}

	m_nFramesPerSecond++;
	m_fFPSTimeElapsed += fTimeElapsed;
	if (m_fFPSTimeElapsed > 1.0f)
	{
		m_nCurrentFrameRate = m_nFramesPerSecond;
		m_nFramesPerSecond = 0;
		m_fFPSTimeElapsed = 0.0f;
	}

	m_fTimeElapsed = 0.0f;
	for (ULONG i = 0; i < m_nSampleCount; i++)
		m_fTimeElapsed += m_fFrameTime[i];
	if (m_nSampleCount > 0)
		m_fTimeElapsed /= m_nSampleCount;

	deltaTime = m_fTimeElapsed;
}

//unsigned long Time::GetFrameRate(LPTSTR lpszString, int nCharacters)
//{
//	if (lpszString)
//	{
//		_itow_s(m_nCurrentFrameRate, lpszString, nCharacters, 10);
//		wcscat_s(lpszString, nCharacters, _T(" FPS)"));
//	}
//
//	return m_nCurrentFrameRate;
//}

float Time::GetTimeElapsed()
{
	return m_fTimeElapsed;
}
