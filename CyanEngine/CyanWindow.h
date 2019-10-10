#pragma once

class CyanWindow : public Singleton<CyanWindow>
{
public:
	// 윈도우 구성 요소, 분리할 필요 있음
	static HINSTANCE m_hInstance;
	static HWND m_hWnd;

	static int m_nWndClientWidth;
	static int m_nWndClientHeight;
};