#include "pch.h"
#include "CyanWindow.h"

HINSTANCE CyanWindow::m_hInstance = nullptr;
HWND CyanWindow::m_hWnd = nullptr;
int CyanWindow::m_nWndClientWidth = FRAME_BUFFER_WIDTH;
int CyanWindow::m_nWndClientHeight = FRAME_BUFFER_HEIGHT;