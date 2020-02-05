#include "pch.h"
#include "CyanApp.h"

HWND CyanApp::hwnd{ nullptr };

int CyanApp::Run(HINSTANCE hInstance, int nCmdShow)
{
	return 0;
}

LRESULT CALLBACK CyanApp::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}