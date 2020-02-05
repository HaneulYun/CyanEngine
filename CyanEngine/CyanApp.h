#pragma once

class CyanApp
{
private:
	static HWND hwnd;

public:
	static int Run(CyanFW* cyanFW, HINSTANCE hInstance, int nCmdShow);
	static HWND GetHwnd() { return hwnd; }

protected:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
