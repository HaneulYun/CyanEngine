#pragma once
#include "Time.h"

class CyanFW
{
private:
	Scene* scene{ nullptr };

public:
	CyanFW();
	~CyanFW();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);

	void OnDestroy();

	void ProcessInput();
	void FrameAdvance();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

