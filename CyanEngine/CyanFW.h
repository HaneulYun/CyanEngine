#pragma once
#include "Time.h"

class CyanFW
{
private:
	Scene* scene{ nullptr };

	// Time
	_TCHAR m_pszFrameRate[50];

public:
	CyanFW();
	~CyanFW();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	int Run();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

