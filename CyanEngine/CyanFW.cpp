#include "pch.h"
#include "CyanFW.h"

CyanFW::CyanFW()
{
	_tcscpy_s(m_pszFrameRate, _T("CyanEngine ("));
}

CyanFW::~CyanFW()
{
}

bool CyanFW::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	CyanWindow::m_hInstance = hInstance;
	CyanWindow::m_hWnd = hMainWnd;

	scene = new Scene();
	if (scene)
		scene->Start();

	Time::Instance()->Reset();
	
	return true;
}

void CyanFW::OnDestroy()
{
	if (scene)
		scene->Destroy();
}

void CyanFW::FrameAdvance()
{
	Time::Instance()->Tick();

	scene->Update();
	scene->Render();

	Time::Instance()->GetFrameRate(m_pszFrameRate + 12, 37);
	SetWindowText(CyanWindow::m_hWnd, m_pszFrameRate);
}

void CyanFW::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	default:
		break;
	}
}

void CyanFW::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_RETURN:
		case VK_F8:
			break;
		case VK_F9:
			RendererManager::Instance()->ChangeSwapChainState();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CyanFW::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		CyanWindow::m_nWndClientWidth = LOWORD(lParam);
		CyanWindow::m_nWndClientHeight = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}

	return 0;
}
