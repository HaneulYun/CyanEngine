#pragma once

class CyanWindow : public Singleton<CyanWindow>
{
public:
	// ������ ���� ���, �и��� �ʿ� ����
	static HINSTANCE m_hInstance;
	static HWND m_hWnd;

	static int m_nWndClientWidth;
	static int m_nWndClientHeight;
};